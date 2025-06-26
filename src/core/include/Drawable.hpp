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

class Drawable: public MoveableBase<Drawable>  {
 public:
  Drawable(glm::vec3 origin, std::shared_ptr<Configuration>context);
  virtual void draw(Uniforms& unniforms) = 0;
  virtual ~Drawable() = default;

  inline void setOrigin(glm::vec3 origin) {
    transform = glm::translate(glm::mat4(1.0f), origin);
  };

  inline glm::mat4 getTransform() const { return transform; };
 
  inline std::shared_ptr<Configuration> getContext() {
    if(auto locked = this->context.lock()){
      return locked;
    }
    return nullptr;
    }
  void activate() override ;
  void deactivate() override ;

  std::function<glm::mat4(Drawable& drawable, float time)> preDraw;
  std::function<void(Drawable& drawable)> postDraw;
  
  protected:

  virtual void drawNormals(Uniforms& uniforms, glm::mat4 transform) = 0;
  virtual void drawMesh(Uniforms& uniforms, glm::mat4 transform) = 0;

 private:
  glm::mat4 transform = glm::mat4(1.0);

  std::weak_ptr<Configuration> context;

  friend MoveableBase<Drawable>;
  Moveable moveable;
};
