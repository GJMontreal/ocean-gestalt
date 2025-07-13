#pragma once

#include "Uniforms.hpp"
#include "Moveable.hpp"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

class ShaderProgram;
class Configuration;

struct DrawableVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec4 color;
};

struct Transform {
  glm::vec3 displacement;
  glm::quat rotation;
  glm::vec3 scale;
};

class Drawable: public MoveableBase<Drawable>  {
 public:
  Drawable(glm::vec3 origin, std::shared_ptr<Configuration>context);
  virtual void draw(Uniforms& uniforms) = 0;
  virtual ~Drawable() = default;

  inline void setOrigin(glm::vec3 origin) {
    position = origin;
  };

  inline void setScale(glm::vec3 aScale) {
    scale = aScale;
  }
  inline glm::vec3 getPosition(){return position;};
  inline glm::vec3 getScale() { return scale;};

  inline glm::mat4 getTransform() const { return glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f),scale); };
  
  inline std::shared_ptr<Configuration> getContext() {
    if(auto locked = this->context.lock()){
      return locked;
    }
    return nullptr;
    }
  
  void activate() override ;
  void deactivate() override ;

  void setIfShouldDraw(bool aBool) { shouldDraw = aBool; }

  bool getIfShouldDraw() { return shouldDraw; }

  std::function<Transform(Drawable& drawable, float time)> preDraw;
  // std::function<void(Drawable& drawable)> postDraw;  //TODO: remove if we're not using this
  
  protected:

  virtual void drawNormals(Uniforms& uniforms, Transform transform) = 0;
  virtual void drawMesh(Uniforms& uniforms, Transform transform) = 0;

 private:
  // glm::mat4 transform = glm::mat4(1.0);
  glm::vec3 position;
  glm::vec3 scale = glm::vec3(1.0f);
  glm::vec3 rotation;

  std::weak_ptr<Configuration> context;

  //TODO: not all drawables should be moveable
  friend MoveableBase<Drawable>;
  Moveable moveable;

  bool shouldDraw;
};
