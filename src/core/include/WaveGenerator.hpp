#pragma once

#include "ApiAdapter.hpp"
#include "Configuration.hpp"
#include <glm/glm.hpp>


struct Wind{
  glm::vec2 direction;
  float speed;
};
class WaveGenerator {
 public:
  WaveGenerator(std::shared_ptr<Configuration> configuration, std::shared_ptr<ApiAdapter> api);
  WaveGenerator(int numWaves, Wind& wind, std::shared_ptr<ApiAdapter> api) ;
};

float randf(float min, float max);

