#pragma once

#include "Drawable.hpp"
#include "Wave.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <random>

class Configuration;
struct Uniforms;

struct SprayParticle {
    glm::vec3 worldPos  = glm::vec3(0.f);
    glm::vec3 velocity  = glm::vec3(0.f);
    float     lifetime    = 0.f;
    float     maxLifetime = 1.f;
    float     size        = 0.2f;
    float     seed        = 0.f;
    bool      active      = false;
};

class SprayParticleSystem : public Drawable {
public:
    explicit SprayParticleSystem(std::shared_ptr<Configuration> configuration);
    ~SprayParticleSystem() override;

    void draw(Uniforms& uniforms) override;

protected:
    void drawNormals(Uniforms&, Transform) override {}
    void drawMesh(Uniforms&, Transform) override {}

private:
    void update(float dt, float time);
    void spawnParticles(float time);
    void uploadInstances();
    void initGPUBuffers();

    static constexpr int   MAX_PARTICLES      = 1500;
    static constexpr float SPAWN_THRESHOLD    = 0.45f;
    static constexpr int   GRID_SIZE          = 16;
    static constexpr float OCEAN_HALF_EXTENT  = 40.0f;
    static constexpr float GRAVITY            = 9.81f;

    std::vector<SprayParticle> particles;
    float lastTime  = -1.f;
    int   activeCount = 0;

    GLuint quadVAO     = 0;
    GLuint quadVBO     = 0;
    GLuint instanceVBO = 0;

    std::mt19937 rng{42};
    std::uniform_real_distribution<float> randF{0.f, 1.f};
};
