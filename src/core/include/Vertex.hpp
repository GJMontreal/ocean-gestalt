
#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>

// We want to standardize what we're passing to the shaders as much as possible
struct VertexType {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec4 color;
};

#endif
