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
  void setSpinParameters(float torqueScale, float damping, float stiffness) {
    spinTorqueScale = torqueScale;
    angularDamping  = damping;
    torsionalStiffness = stiffness;
  }

private:
  friend MoveableBase<Prop>;
  Moveable moveable;
  glm::quat lastRotation;

  std::shared_ptr<Drawable> drawable;
  std::weak_ptr<Configuration> context;

  // 0.0 = rigid upright, 1.0 = fully follows wave surface.
  float rightingWeight = 0.5f;

  // spin state
  float angularVelocity = 0.0f;
  float currentYaw      = 0.0f;
  float lastTime        = -1.0f;

  // spin config (set from scene.json via setSpinParameters)
  float spinTorqueScale    = 2.0f;
  float angularDamping     = 1.5f;
  float torsionalStiffness = 0.5f;
};
