#include "Drawable.hpp"

Drawable::Drawable(std::shared_ptr<Configuration> context)
    : context(context), moveable(std::make_shared<Moveable>()) {
  moveable->onPositionChanged = [&](const glm::vec3& pos) { setOrigin(pos);
  };
}
