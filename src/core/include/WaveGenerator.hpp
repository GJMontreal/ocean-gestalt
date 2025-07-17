#pragma once

#include "ApiAdapter.hpp"
#include <glm/glm.hpp>


struct Wind{
  glm::vec2 direction;
  float speed;
};
class WaveGenerator {
 public:
  WaveGenerator(int numWaves, Wind& wind, std::shared_ptr<ApiAdapter> api) ;
};

float randf(float min, float max);

