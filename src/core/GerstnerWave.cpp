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
    return totalDisplacement;// / glm::vec3(waves.size());
}

/*
From our gerstner shader
const float VELOCITY_SCALE = 1.0; // Scale spatial dimensions and wave wavelengths
const float PI = 3.14159265358979323;
vec3 waveOffset(float time, vec3 aPosition, WAVE wave) {
    float k = 2.0 * PI / wave.wavelength; //what happens when wavelength is zero?
    float w = VELOCITY_SCALE * sqrt(GRAVITY * k); // only frequency is scaled
    vec2 D = normalize(wave.direction.xy);

    float phase = dot(D * k, aPosition.xz) - w * time;

    float S = sin(phase);
    float C = cos(phase);

    float y = wave.amplitude * C;
    vec2 xz = aPosition.xz - D * wave.steepness * S * wave.amplitude;

    return vec3(xz.x, y, xz.y);
}


vec3 calcNewPosition(vec3 aPosition){
  vec3 offset = vec3(0.0);
  for(int i=0; i < NUM_WAVES; i++){
    vec3 newOffset = waveOffset(time, aPosition, waves[i] );
    offset += newOffset;
  }
  offset = aPosition + offset / float(NUM_WAVES);
  return offset;
}
*/