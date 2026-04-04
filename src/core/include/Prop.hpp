#pragma once

#include "Drawable.hpp"
#include "Moveable.hpp"

#include <glm/gtc/quaternion.hpp>
#include <memory>

class Configuration;

class Prop : public MoveableBase<Prop> {
public:
  Prop(std::shared_ptr<Drawable> drawable, glm::vec3 origin, std::shared_ptr<Configuration> context);

  std::shared_ptr<Drawable> getDrawable() { return drawable; }
  std::shared_ptr<Configuration> getContext() { return context.lock(); }

  void activate() override {}
  void deactivate() override {}
  void setRightingWeight(float w) { rightingWeight = w; }

private:
  friend MoveableBase<Prop>;
  Moveable moveable;
  glm::quat lastRotation;

  std::shared_ptr<Drawable> drawable;
  std::weak_ptr<Configuration> context;

  // 0.0 = rigid upright, 1.0 = fully follows wave surface. Will become a
  // per-prop JSON config field when scene description is added.
  float rightingWeight = 0.5f;
};
