#include "Light.hpp"
#include "Camera.hpp"
#include "Configuration.hpp"

Light::Light(glm::vec3 origin, std::shared_ptr<Configuration> context) : context(context){

  sphere = std::make_shared<Sphere>(origin, glm::vec4(1.f,1.f,0.f,1.f), context);
  sphere->enableDrawing(false);
  moveable.onPositionChanged = [&](const glm::vec3& pos) {
    this->setOrigin(pos);
    this->sphere->setOrigin(pos);
  };

  moveable.setPosition(origin);  //important we don't use getMoveable here as it will refer to the base class
  moveable.getMoveDirection = [this]()->MoveDirection&{
    return this->getContext()->camera->getMoveDirection(); //capturing context directly fails in here. It must be changing
  };
}


void Light::activate() {
  this->sphere->enableDrawing(true);
}

void Light::deactivate() {
  this->sphere->enableDrawing(false);
}