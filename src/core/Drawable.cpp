#include "Drawable.hpp"

#include "Camera.hpp"
#include "Configuration.hpp"

Drawable::Drawable(glm::vec3 origin, std::shared_ptr<Configuration> context) : context(context) {
  moveable.onPositionChanged = [&](const glm::vec3& pos) {
    this->setOrigin(pos);
  };
  moveable.setPosition(origin);  //important we don't use getMoveable here as it will refer to the base class
  moveable.getMoveDirection = [this]()->MoveDirection&{
    return this->getContext()->camera->getMoveDirection(); //capturing context directly fails in here. It must be changing
  };
}

void Drawable::activate() {
  /* do nothing */
}

void Drawable::deactivate() {
  /* do nothing */
}