#ifndef __UNIFORMS_HPP
#define __UNIFORMS_HPP

#include "glm/glm.hpp"

struct Uniforms{
  glm::mat4 projection;
  glm::mat4 view;
  float time;
  glm::mat4 reflectionMatrix = glm::mat4(1.0f);
  unsigned int reflectionTexture = 0;
};

#endif
