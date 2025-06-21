#pragma once

#include "Uniforms.hpp"

#include <memory>

class ShaderProgram;

struct DrawableVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color; // not sure if we need this
};

class Drawable {
 public:
  virtual void draw(Uniforms& uniforms) = 0;
  virtual ~Drawable() = default;

  inline glm::vec3 getPosition() const { return position;};
  inline void setPosition(glm::vec3 position) { this->position = position;};
 private:
  glm::vec3 position;  // is this the centroid?
};
