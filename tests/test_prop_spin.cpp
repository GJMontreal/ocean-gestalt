#include "doctest.h"
#include "GerstnerWave.hpp"
#include "Wave.hpp"

#include <cmath>
#include <memory>
#include <vector>

// Matches the curl computation in Prop.cpp preDraw:
//   curlY = ((dx.z - d.z) - (dz.x - d.x)) / eps
static float computeCurl(const std::vector<std::shared_ptr<Wave>>& waves,
                         glm::vec2 pos, float time, float eps = 0.1f) {
    auto d  = evaluateGerstnerWaves(waves, pos,                          time);
    auto dx = evaluateGerstnerWaves(waves, pos + glm::vec2(eps, 0.0f),   time);
    auto dz = evaluateGerstnerWaves(waves, pos + glm::vec2(0.0f, eps),   time);
    return ((dx.z - d.z) - (dz.x - d.x)) / eps;
}

// The noise torque formula proposed to replace the curl approach.
static float noiseTorque(glm::vec2 pos, float time) {
    float phase1 = pos.x * 0.07f + time * 0.13f;
    float phase2 = pos.y * 0.05f + time * 0.09f;
    return std::sin(phase1) * std::cos(phase2);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::shared_ptr<Wave> makeWave(float amplitude, float wavelength,
                                      float steepness, glm::vec2 direction,
                                      float phase = 0.0f) {
    auto w = std::make_shared<Wave>();
    w->amplitude  = amplitude;
    w->wavelength = wavelength;
    w->steepness  = steepness;
    w->direction  = direction;
    w->phase      = phase;
    return w;
}

// ---------------------------------------------------------------------------
// Curl tests — document why the original approach produced no spin
// ---------------------------------------------------------------------------

TEST_CASE("Gerstner curl: single wave propagating in X has zero curl") {
    // A plane wave with direction (1,0) should have exactly zero XZ curl.
    auto waves = std::vector<std::shared_ptr<Wave>>{
        makeWave(1.0f, 20.0f, 0.5f, {1.0f, 0.0f})
    };
    // Test at several positions and times.
    for (float t : {0.0f, 1.0f, 5.0f, 10.0f}) {
        for (auto pos : {glm::vec2(0,0), glm::vec2(5,3), glm::vec2(-7,2)}) {
            CHECK(computeCurl(waves, pos, t) == doctest::Approx(0.0f).epsilon(1e-4));
        }
    }
}

TEST_CASE("Gerstner curl: single wave propagating at 45 degrees has zero curl") {
    auto waves = std::vector<std::shared_ptr<Wave>>{
        makeWave(1.0f, 30.0f, 0.8f, glm::normalize(glm::vec2(1.0f, 1.0f)))
    };
    for (float t : {0.0f, 2.5f, 7.0f}) {
        for (auto pos : {glm::vec2(0,0), glm::vec2(4,-3)}) {
            CHECK(computeCurl(waves, pos, t) == doctest::Approx(0.0f).epsilon(1e-4));
        }
    }
}

TEST_CASE("Gerstner curl: superposition of waves in different directions still has zero curl") {
    // Multiple crossing waves — the original assumption was that these would
    // produce non-zero curl. They don't: each plane wave contributes zero and
    // the sum is zero.
    auto waves = std::vector<std::shared_ptr<Wave>>{
        makeWave(0.5f, 20.0f, 0.5f, {1.0f, 0.0f}),
        makeWave(0.3f, 15.0f, 0.7f, glm::normalize(glm::vec2(0.7f, 0.3f))),
        makeWave(0.4f, 25.0f, 0.6f, glm::normalize(glm::vec2(-0.5f, 0.8f))),
        makeWave(0.2f, 10.0f, 1.0f, {0.0f, 1.0f}),
    };
    for (float t : {0.0f, 3.0f, 8.5f}) {
        for (auto pos : {glm::vec2(0,0), glm::vec2(10,5), glm::vec2(-3,8)}) {
            CHECK(computeCurl(waves, pos, t) == doctest::Approx(0.0f).epsilon(1e-3));
        }
    }
}

// ---------------------------------------------------------------------------
// Noise torque tests — verify the replacement approach
// ---------------------------------------------------------------------------

TEST_CASE("Noise torque: output is in [-1, 1]") {
    for (float t : {0.0f, 1.0f, 10.0f, 100.0f}) {
        for (auto pos : {glm::vec2(0,0), glm::vec2(5,3), glm::vec2(-7,12)}) {
            float v = noiseTorque(pos, t);
            CHECK(v >= -1.0f);
            CHECK(v <=  1.0f);
        }
    }
}

TEST_CASE("Noise torque: varies with time at a fixed position") {
    glm::vec2 pos(5.0f, 3.0f);
    // Collect values over time; they should not all be equal.
    std::vector<float> values;
    for (float t = 0.0f; t < 50.0f; t += 0.5f)
        values.push_back(noiseTorque(pos, t));

    float minVal = *std::min_element(values.begin(), values.end());
    float maxVal = *std::max_element(values.begin(), values.end());
    CHECK(maxVal - minVal > 0.5f);  // meaningful variation over time
}

TEST_CASE("Noise torque: varies with position at a fixed time") {
    float t = 5.0f;
    std::vector<float> values;
    for (float x = 0.0f; x < 50.0f; x += 2.0f)
        values.push_back(noiseTorque({x, 0.0f}, t));

    float minVal = *std::min_element(values.begin(), values.end());
    float maxVal = *std::max_element(values.begin(), values.end());
    CHECK(maxVal - minVal > 0.5f);  // meaningful spatial variation
}

TEST_CASE("Noise torque: two nearby positions produce different values") {
    float t = 3.0f;
    float v1 = noiseTorque({0.0f,  0.0f}, t);
    float v2 = noiseTorque({5.0f,  0.0f}, t);
    float v3 = noiseTorque({0.0f, 10.0f}, t);
    CHECK(v1 != doctest::Approx(v2));
    CHECK(v1 != doctest::Approx(v3));
}
