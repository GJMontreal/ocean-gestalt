#include "SprayParticleSystem.hpp"
#include "Configuration.hpp"
#include "GerstnerWave.hpp"
#include "Shader.hpp"
#include "Uniforms.hpp"

#include <glm/glm.hpp>
#include <cstring>
#include <cmath>

// Packed per-instance data uploaded to the GPU each frame
struct ParticleInstance {
    float x, y, z;
    float lifetimeFrac;
    float size;
    float vx, vy, vz;   // normalised velocity direction for billboard orientation
    float seed;          // random 0-1 for per-particle shape variation
};

SprayParticleSystem::SprayParticleSystem(std::shared_ptr<Configuration> configuration)
    : Drawable(glm::vec3(0.f), configuration)
{
    particles.resize(MAX_PARTICLES);
    initGPUBuffers();
}

SprayParticleSystem::~SprayParticleSystem() {
    if (quadVAO) glDeleteVertexArrays(1, &quadVAO);
    if (quadVBO) glDeleteBuffers(1, &quadVBO);
    if (instanceVBO) glDeleteBuffers(1, &instanceVBO);
}

void SprayParticleSystem::initGPUBuffers() {
    // Four corners of a unit quad as a triangle strip
    // (-1,-1) (1,-1) (-1,1) (1,1)
    const float corners[] = {
        -1.f, -1.f,
         1.f, -1.f,
        -1.f,  1.f,
         1.f,  1.f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(quadVAO);

    // Static quad corners — attribute 0
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(corners), corners, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glVertexAttribDivisor(0, 0);  // per-vertex

    // Per-instance data — attributes 1, 2, 3
    const GLsizei stride = static_cast<GLsizei>(sizeof(ParticleInstance));
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * stride, nullptr, GL_DYNAMIC_DRAW);

    // worldPos (vec3)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ParticleInstance, x));
    glVertexAttribDivisor(1, 1);

    // lifetimeFrac (float)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ParticleInstance, lifetimeFrac));
    glVertexAttribDivisor(2, 1);

    // size (float)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ParticleInstance, size));
    glVertexAttribDivisor(3, 1);

    // normalised velocity direction (vec3)
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ParticleInstance, vx));
    glVertexAttribDivisor(4, 1);

    // shape seed (float)
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ParticleInstance, seed));
    glVertexAttribDivisor(5, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SprayParticleSystem::update(float dt, float time) {
    for (auto& p : particles) {
        if (!p.active) continue;
        // Gravity is universal — same acceleration regardless of mass
        p.velocity.y -= GRAVITY * 0.9f * dt;

        // Drag opposes velocity and scales inversely with mass:
        // light particles (mist) decelerate quickly and linger;
        // heavy particles (droplets) hold speed and arc farther
        static constexpr float DRAG_K = 0.30f;
        float drag = 1.0f - (DRAG_K / p.mass) * dt;
        p.velocity *= std::max(drag, 0.0f);

        // Swirl: rotate horizontal velocity by swirl * dt radians each frame
        float sa = p.swirl * dt;
        float cs = std::cos(sa), sn = std::sin(sa);
        float vx = cs * p.velocity.x - sn * p.velocity.z;
        float vz = sn * p.velocity.x + cs * p.velocity.z;
        p.velocity.x = vx;
        p.velocity.z = vz;

        p.worldPos    += p.velocity * dt;
        p.lifetime    -= dt;
        if (p.lifetime <= 0.f) p.active = false;
    }
    spawnParticles(time);
}

void SprayParticleSystem::spawnParticles(float time) {
    auto ctx = getContext();
    if (!ctx) return;
    auto& waves = ctx->waves;

    static constexpr float PI = 3.14159265f;

    float step = (2.f * OCEAN_HALF_EXTENT) / GRID_SIZE;

    for (int gz = 0; gz < GRID_SIZE; ++gz) {
        for (int gx = 0; gx < GRID_SIZE; ++gx) {
            float jx = (randF(rng) - 0.5f) * step;
            float jz = (randF(rng) - 0.5f) * step;
            float sx = -OCEAN_HALF_EXTENT + (gx + 0.5f) * step + jx;
            float sz = -OCEAN_HALF_EXTENT + (gz + 0.5f) * step + jz;
            glm::vec2 pos(sx, sz);

            // Per-wave crest detection: cos(phase)=1 at a Gerstner crest.
            // Accumulate amplitude-weighted crestness and a wave-physics launch vector.
            float crestWeight = 0.f;
            float totalWeight = 0.f;
            glm::vec2 launchDir(0.f);
            float launchSpeed = 0.f;

            for (auto& w : waves) {
                if (w->amplitude < 0.001f || w->wavelength < 0.001f) continue;
                if (glm::length(w->direction) < 0.001f) continue;

                float k     = 2.f * PI / w->wavelength;
                float omega = std::sqrt(GRAVITY * k);
                glm::vec2 dir = glm::normalize(w->direction);

                float phase     = glm::dot(dir * k, pos) - omega * time + w->phase;
                float crestness = std::cos(phase);          // 1 at crest, -1 at trough

                // Weight by amplitude * steepness so tall steep waves drive more spray
                float wgt = w->amplitude * w->steepness;
                totalWeight  += wgt;
                crestWeight  += wgt * std::max(0.f, crestness);

                // Weighted average of wave propagation direction and phase speed
                float phaseSpeed = omega / k;
                launchDir   += dir * (wgt * std::max(0.f, crestness));
                launchSpeed += phaseSpeed * (wgt * std::max(0.f, crestness));
            }

            if (totalWeight < 0.001f) continue;
            crestWeight /= totalWeight;   // normalised 0–1

            // Only spawn near actual crests of reasonably steep waves
            if (crestWeight < SPAWN_THRESHOLD) continue;

            // Probabilistic rate control: more likely the higher the crest
            float spawnProb = (crestWeight - SPAWN_THRESHOLD) / (1.f - SPAWN_THRESHOLD);
            if (randF(rng) > spawnProb * 0.45f) continue;

            SprayParticle* slot = nullptr;
            for (auto& p : particles) {
                if (!p.active) { slot = &p; break; }
            }
            if (!slot) return;

            // Actual displaced surface position (Gerstner horizontal shift included)
            glm::vec3 disp = evaluateGerstnerWaves(waves, pos, time);
            glm::vec3 spawnPos(sx + disp.x, disp.y, sz + disp.z);

            // Launch velocity: fraction of wave phase speed along travel dir, plus upward burst
            glm::vec2 ld = (glm::length(launchDir) > 0.001f)
                           ? glm::normalize(launchDir)
                           : glm::vec2(0.f, 1.f);
            float ls = (totalWeight > 0.f) ? launchSpeed / totalWeight : 2.f;
            // Spray is ejected primarily along the wave's travel direction.
            // Orbital velocity at the crest is ~A*omega, mostly horizontal.
            // Upward component is a small fraction — gravity quickly wins.
            float lateral = ls * (0.8f + randF(rng) * 0.8f);
            float upSpeed = ls * (0.15f + crestWeight * 0.35f + randF(rng) * 0.2f);

            slot->worldPos    = spawnPos;
            slot->velocity    = glm::vec3(ld.x * lateral, upSpeed, ld.y * lateral);
            slot->maxLifetime = 1.2f + crestWeight * 1.0f + randF(rng) * 1.0f;
            slot->lifetime    = slot->maxLifetime;
            slot->mass        = 0.3f + randF(rng) * 2.2f;
            slot->size        = 0.2f + slot->mass * 0.25f + randF(rng) * 0.2f;
            slot->seed        = randF(rng);
            slot->swirl       = (randF(rng) - 0.5f) * 8.0f;
            slot->active      = true;
        }
    }
}

void SprayParticleSystem::uploadInstances() {
    std::vector<ParticleInstance> instances;
    instances.reserve(MAX_PARTICLES);

    for (auto& p : particles) {
        if (!p.active) continue;
        float frac = p.lifetime / p.maxLifetime;
        glm::vec3 vn = glm::length(p.velocity) > 0.001f
                       ? glm::normalize(p.velocity) : glm::vec3(0, 1, 0);
        instances.push_back({ p.worldPos.x, p.worldPos.y, p.worldPos.z,
                               frac, p.size, vn.x, vn.y, vn.z, p.seed });
    }

    activeCount = static_cast<int>(instances.size());
    if (activeCount == 0) return;

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    activeCount * static_cast<GLsizeiptr>(sizeof(ParticleInstance)),
                    instances.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SprayParticleSystem::draw(Uniforms& uniforms) {
    float dt = 0.f;
    if (lastTime >= 0.f && uniforms.time > lastTime) {
        dt = uniforms.time - lastTime;
    }
    lastTime = uniforms.time;

    update(dt, uniforms.time);
    uploadInstances();

    if (activeCount == 0) return;

    auto ctx = getContext();
    if (!ctx) return;
    auto shader = ctx->getShader("spray_particle");
    if (!shader) return;

    shader->activate();
    shader->setUniform("projection", uniforms.projection);
    shader->setUniform("view",       uniforms.view);

    // additive blending: spray droplets add brightness like sunlit mist
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);

    glBindVertexArray(quadVAO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, activeCount);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader->deactivate();
}
