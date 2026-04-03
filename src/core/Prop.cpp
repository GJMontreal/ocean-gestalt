#include "Prop.hpp"

#include "Camera.hpp"
#include "Configuration.hpp"
#include "GerstnerWave.hpp"
#include "Moveable.hpp"

#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>

Prop::Prop(std::shared_ptr<Drawable> drawable, glm::vec3 origin, std::shared_ptr<Configuration> context)
    : drawable(drawable), context(context) {
  lastRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

  moveable.onPositionChanged = [&](const glm::vec3& pos) {
    this->drawable->setOrigin(pos);
  };

  moveable.setPosition(origin);
  moveable.getMoveDirection = [this]() -> MoveDirection& {
    return this->getContext()->camera->getMoveDirection();
  };

  drawable->preDraw = [this](Drawable& _, float time) -> Transform {
    auto& position = getDrawable()->getPosition();
    auto positionXZ = glm::vec2(position.x, position.z);
    auto displacement = glm::vec3(0.0f);
    glm::vec3 targetNormal = glm::vec3(0.0f, 1.0f, 0.0f);

    if (getMoveable().getIsFloating()) {
      displacement = evaluateGerstnerWaves(getContext()->getWaves(), positionXZ, time);
      auto dx = evaluateGerstnerWaves(getContext()->getWaves(), positionXZ + glm::vec2(0.1f, 0.0f), time);
      auto dz = evaluateGerstnerWaves(getContext()->getWaves(), positionXZ + glm::vec2(0.0f, 0.1f), time);

      // Build full surface tangents (base step + displacement delta) so the
      // normal is always upward-facing and varies smoothly — no sign flips.
      glm::vec3 surfaceTangentX = glm::vec3(0.1f, 0.0f, 0.0f) + (dx - displacement);
      glm::vec3 surfaceTangentZ = glm::vec3(0.0f, 0.0f, 0.1f) + (dz - displacement);

      // cross(Z, X) gives the upward surface normal in Y-up convention.
      glm::vec3 rawNormal = glm::cross(surfaceTangentZ, surfaceTangentX);
      float normalLen = glm::length(rawNormal);
      if (normalLen > 1e-5f) {
        glm::vec3 waveNormal = rawNormal / normalLen;
        // Blend toward world-up to simulate a weighted keel.
        // rightingWeight: 0.0 = rigid upright, 1.0 = fully follows wave.
        targetNormal = glm::normalize(
            glm::mix(glm::vec3(0.0f, 1.0f, 0.0f), waveNormal, rightingWeight));
      }
    }

    glm::vec3 prevUp = lastRotation * glm::vec3(0.0f, 1.0f, 0.0f);
    glm::quat rAlign = glm::rotation(prevUp, targetNormal);
    glm::quat rotation = glm::normalize(rAlign * lastRotation);
    lastRotation = rotation;

    return {displacement, rotation, glm::vec3(1.0f)};
  };
}
