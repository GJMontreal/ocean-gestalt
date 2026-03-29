#pragma once

#include "ApiAdapter.hpp"
#include "Configuration.hpp"

class WaveGenerator {
 public:
  WaveGenerator(std::shared_ptr<Configuration> configuration, std::shared_ptr<ApiAdapter> api);
};

float randf(float min, float max);
