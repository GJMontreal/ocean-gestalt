#pragma once

#include <glm/glm.hpp>

// We want to standardize what we're passing to the shaders as much as possible
struct Vertex {
  glm::vec3 position;
  glm::vec2 texCoord;
  glm::vec3 tangent;
};
