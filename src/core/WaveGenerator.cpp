#include "WaveGenerator.hpp"

#include "random.hpp"


#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>


using Random = effolkronium::random_static;

namespace {
constexpr float MIN_WAVELENGTH = 5.0f;           // meters
constexpr float MAX_WAVELENGTH = 100.0f;         // meters
constexpr float MAX_PHYSICAL_STEEPNESS = 0.44f;  // stability limit
constexpr float BASE_STEEPNESS = 0.01f;
constexpr float WIND_STEEPNESS_SCALE = 0.02f;
constexpr float SPREAD_ANGLE_DEGREES = 90.0f;  // total angular spread
constexpr float GRAVITY = 9.81f;
}  // namespace

// Generate a set of waves
WaveGenerator::WaveGenerator(int numWaves, Wind& wind, std::shared_ptr<ApiAdapter> api) {
  auto steepnessFromWind = [](float speed) {
    float s = BASE_STEEPNESS + WIND_STEEPNESS_SCALE * (speed / 10.0f);
    return std::min(s, MAX_PHYSICAL_STEEPNESS);
  };

  float maxSteepness = steepnessFromWind(wind.speed);
  float spread = glm::radians(SPREAD_ANGLE_DEGREES);
  
  // Use log spacing for wavelengths
  float logMin = std::log(MIN_WAVELENGTH);
  float logMax = std::log(MAX_WAVELENGTH);
  float logStep = (logMax - logMin) / (numWaves - 1);

  for (int i = 0; i < numWaves; i++) {
    float logLambda = logMin + i * logStep;
    float lambda = std::exp(logLambda);
    float k = 2.0f * glm::pi<float>() / lambda;
    float omega = std::sqrt(GRAVITY * k);
    
    // Scale steepness linearly or sinusoidally across waves
    float steepness = maxSteepness * (0.5f + 0.5f * std::sin(glm::pi<float>() * i / (numWaves - 1)));
    float amplitude = (steepness * lambda) / (2.0f * glm::pi<float>());

    float angleOffset = -spread / 2.0f + spread * (float(i) / float(numWaves - 1));
    glm::vec2 direction = glm::rotate(wind.direction, angleOffset);
    float phase = randf(0.0f, glm::two_pi<float>());

    std::string pathPrefix = "uniforms.waves[" + std::to_string(i) + "].";
    api->setValue(pathPrefix + "amplitude", amplitude);
    api->setValue(pathPrefix + "wavelength", lambda);
    api->setValue(pathPrefix + "phase", phase);
    api->setValue(pathPrefix + "direction", std::vector<float>{direction.x, direction.y, 0.0f});
    api->setValue(pathPrefix + "steepness", steepness);
  }
}


inline float randf(float min, float max) {
  static thread_local std::mt19937 rng(std::random_device{}());
  std::uniform_real_distribution<float> dist(min, max);
  return dist(rng);
}