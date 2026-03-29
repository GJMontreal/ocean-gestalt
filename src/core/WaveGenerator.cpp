#include "WaveGenerator.hpp"
#include "UniformAnimator.hpp"

#include <random>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace {
constexpr float SPREAD_ANGLE_DEGREES = 30.0f;
}

WaveGenerator::WaveGenerator(std::shared_ptr<Configuration> configuration,
                             std::shared_ptr<ApiAdapter> api) {
  auto anim = configuration->getAnimator();
  int numWaves = static_cast<int>(configuration->waves.size());

  glm::vec2 windDir{randf(-1.0f, 1.0f), randf(-1.0f, 1.0f)};
  windDir = glm::normalize(windDir);
  float spread = glm::radians(SPREAD_ANGLE_DEGREES);

  constexpr float DURATION = 10.0f;

  for (int i = 0; i < numWaves; i++) {
    float angleOffset = randf(-spread / 2.0f, spread / 2.0f);
    glm::vec2 direction = glm::rotate(windDir, angleOffset);

    std::string dirKey = "uniforms.waves[" + std::to_string(i) + "].direction";
    ApiValue fromDir = api->getValue(dirKey)
                           .value_or(std::vector<float>{direction.x, direction.y, 0.0f});
    ApiValue toDir = std::vector<float>{direction.x, direction.y, 0.0f};
    anim->animateTo(fromDir, toDir, DURATION,
                    [api, dirKey](const ApiValue& v) { api->setValue(dirKey, v, false); },
                    dirKey);
  }
}

inline float randf(float min, float max) {
  static thread_local std::mt19937 rng(std::random_device{}());
  std::uniform_real_distribution<float> dist(min, max);
  return dist(rng);
}
