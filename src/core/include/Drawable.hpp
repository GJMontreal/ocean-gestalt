#pragma once

#include "Uniforms.hpp"


#include <glm/gtc/matrix_transform.hpp>
#include <memory>

class ShaderProgram;

struct DrawableVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec4 color; 
};

class Drawable {
 public:
  virtual void draw(Uniforms& uniforms) = 0;
  virtual ~Drawable() = default;

  inline glm::vec3 getPosition() const { return position;};
  inline void setPosition(glm::vec3 position) { this->position = position;
                                                transform = glm::translate(glm::mat4(1.0f), position);};

  inline glm::mat4 getTransform() const { return transform;};
 protected:
   virtual void drawNormals(Uniforms& uniforms) = 0;
   virtual void drawMesh(Uniforms& uniforms) = 0;

  glm::mat4 transform = glm::mat4(1.0);  //make this protected once we have it working
 private:
  glm::vec3 position;  // is this the centroid?

};
