#ifndef __UNIFORMS_HPP
#define __UNIFORMS_HPP

#include "glm/glm.hpp"

struct Uniforms{
  glm::mat4 projection;
  glm::mat4 view;
  float time;
};

#endif
