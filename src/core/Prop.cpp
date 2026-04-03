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
    auto displacement =glm::vec3(0.0);
    auto dx = glm::vec3(0.0);
    auto dz = glm::vec3(0.0);
    
    if (getMoveable().getIsFloating()){
      displacement = evaluateGerstnerWaves(getContext()->getWaves(), positionXZ, time);

      dx = evaluateGerstnerWaves(getContext()->getWaves(), positionXZ + glm::vec2(0.1f, 0.0f), time);
      dz = evaluateGerstnerWaves(getContext()->getWaves(), positionXZ + glm::vec2(0.0f, 0.1f), time);
    }
    auto tangentX = dx - displacement;
    auto tangentZ = dz - displacement;
    auto normal = glm::cross(tangentX, tangentZ);

    // Blend wave surface normal with world up to simulate a weighted keel.
    // The object tilts with the waves but is always pulled back toward upright.
    // 0.0 = rigid upright, 1.0 = fully follows wave. Will be made configurable
    // when JSON scene description is added.

    glm::vec3 waveNormal = glm::normalize(normal);
    glm::vec3 targetNormal = glm::normalize(glm::mix(glm::vec3(0, 1, 0), waveNormal, rightingWeight));

    glm::vec3 prevUp = lastRotation * glm::vec3(0, 0, 0);
    glm::quat rAlign = glm::rotation(prevUp, normal);

    glm::quat rotation = glm::normalize(rAlign * lastRotation);
  
    lastRotation = rotation;
  
    
    return {displacement, rotation, glm::vec3(1.0f)};
  };
}
