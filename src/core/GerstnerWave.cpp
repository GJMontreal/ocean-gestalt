#include "GerstnerWave.hpp"

// Represents a single Gerstner wave
struct GerstnerWave {
    float amplitude;
    float wavelength;
    float speed;
    float steepness;
    glm::vec2 direction;  // Must be normalized
};

static const float PI = 3.14159265358979323;
static const float GRAVITY = 9.81;

// Evaluate a single wave’s contribution
glm::vec3 computeGerstnerDisplacement(std::shared_ptr<Wave> wave, const glm::vec2& positionXZ, float time) {
    if(wave->amplitude == 0 || wave->wavelength == 0){
      return glm::vec3(0.f,0.f,0.f);
    }
    float safeWavelength = glm::max(wave->wavelength, 0.01f);
    float k = 2.0f * PI / safeWavelength;
    float omega = sqrt(GRAVITY * k);  // Deep water dispersion relation (optional: override with wave.speed)
    
    auto direction = glm::normalize(wave->direction); // this could be done once in the wave
    
    float phase = glm::dot(direction * k, positionXZ) - omega * time + wave->phase;

    float sinPhase = sin(phase);
    float cosPhase = cos(phase);

    // Displacement
    glm::vec3 displacement;

    displacement.y = wave->amplitude * cosPhase;

    displacement.x = -1.0 * wave->steepness * direction.x  * sinPhase * wave->amplitude;
    displacement.z = -1.0 * wave->steepness * direction.y * sinPhase * wave->amplitude;
 
    return displacement;
}

// Aggregate multiple waves
glm::vec3 evaluateGerstnerWaves(const std::vector<std::shared_ptr<Wave>> waves, const glm::vec2& positionXZ, float time) {
    glm::vec3 totalDisplacement(0.0f);
    for (const auto& wave : waves) {
        totalDisplacement += computeGerstnerDisplacement(wave, positionXZ, time);
    }
    return totalDisplacement;
}

float getSteepness(std::vector<std::shared_ptr<Wave>> waves, const glm::vec3 &position, float time){
  float delta = 0.1f;
  vec3 offsetX =
      evaluateGerstnerWaves(waves, position + vec3(delta, 0, 0), time);
  vec3 offsetZ =
      evaluateGerstnerWaves(waves, position + vec3(0, 0, delta), time);
  vec3 center = evaluateGerstnerWaves(waves, position, time);

  vec3 dX = offsetX - center;
  vec3 dZ = offsetZ - center;

  float steepness = glm::length(dX) + glm::length(dZ);  // crude proxy
  return steepness;
}