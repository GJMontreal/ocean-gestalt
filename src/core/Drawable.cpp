#include "Drawable.hpp"

#include "Configuration.hpp"

Drawable::Drawable(glm::vec3 origin, std::shared_ptr<Configuration> context) : context(context) {
  moveable.onPositionChanged = [&](const glm::vec3& pos) {
    this->setOrigin(pos);
  };
  moveable.setPosition(origin);  //important we don't use getMoveable here as it will refer to the base class

  std::cout << "context (outside) " << &context << std::endl;
  std::cout << "context.get() (outside) " << context.get() << std::endl;
  moveable.getMoveDirection = [&]()->MoveDirection&{
    std::cout << "context.get() (inside) " << getContext().get() << std::endl;
    return getContext()->camera->getMoveDirection();
  };
}
