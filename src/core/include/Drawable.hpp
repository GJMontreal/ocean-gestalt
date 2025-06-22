#pragma once

#include "Uniforms.hpp"
#include "Moveable.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

class ShaderProgram;
class Configuration;

struct DrawableVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec4 color;
};

class Drawable:  {
 public:
  Drawable(std::shared_ptr<Configuration>context);
  virtual void draw(Uniforms& unniforms) = 0;
  virtual ~Drawable() = default;

  inline void setOrigin(glm::vec3 origin) {
    transform = glm::translate(glm::mat4(1.0f), origin);
  };

  inline glm::mat4 getTransform() const { return transform; };
  inline std::shared_ptr<Moveable> getMoveable() {return moveable;};
  inline std::shared_ptr<Configuration> getContext() {return context;};

  protected:
  virtual void drawNormals(Uniforms& uniforms) = 0;
  virtual void drawMesh(Uniforms& uniforms) = 0;

 private:
  glm::mat4 transform = glm::mat4(1.0);

  std::shared_ptr<Configuration> context;

  std::shared_ptr<Moveable> moveable;
};
