#pragma once

#include "Wave.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <cmath>

glm::vec3 computeGerstnerDisplacement(std::shared_ptr<Wave> wave, const glm::vec2& positionXZ, float time);
glm::vec3 evaluateGerstnerWaves(std::vector<std::shared_ptr<Wave>> waves, const glm::vec2& positionXZ, float time);
