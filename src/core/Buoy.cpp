#include "Buoy.hpp"

#include "Camera.hpp"
#include "Configuration.hpp"
#include "Moveable.hpp"
#include "Sphere.hpp"
#include "GerstnerWave.hpp"

#include <glm/gtc/quaternion.hpp>

#include <glm/gtx/vector_angle.hpp>

Buoy::Buoy(glm::vec3 origin, std::shared_ptr<Configuration> context) : context(context){

  sphere = std::make_shared<Sphere>(origin, glm::vec4(0.85f,0.31f,0.f,1.f), context,2.0f);
  lastRotation = glm::angleAxis(glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 1.0f));
  moveable.onPositionChanged = [&](const glm::vec3& pos) {
    this->sphere->setOrigin(pos);
  };

  moveable.setPosition(origin);  //important we don't use getMoveable here as it will refer to the base class
  moveable.getMoveDirection = [this]()->MoveDirection&{
    return this->getContext()->camera->getMoveDirection(); //capturing context directly fails in here. It must be changing
  };

  sphere->preDraw = [this](Drawable& drawable, float time)->Transform{
     auto positionXZ = glm::vec2(getMoveable().position.x, getMoveable().position.z);
     auto displacement = evaluateGerstnerWaves(this->getContext()->getWaves(), positionXZ, time);
    
    //calculate the normal and use this to drive a rotation
    auto dx = evaluateGerstnerWaves(this->getContext()->getWaves(), positionXZ + glm::vec2(0.1f,0.0f), time);
    auto dz = evaluateGerstnerWaves(this->getContext()->getWaves(), positionXZ + glm::vec2(0.0f,0.1f), time);
  
    auto tangentX = dx - displacement;
    auto tangentZ = dz - displacement;
    auto normal = glm::cross(tangentX, tangentZ);
    
    glm::vec3 prevUp = lastRotation * glm::vec3(0, 1, 0);  // what "up" was last frame
    glm::vec3 newUp  = normal;                      // your computed surface normal

    glm::quat rAlign = glm::rotation(prevUp, newUp);       // minimal rotation to match normal
    glm::quat rotation = glm::normalize(rAlign * lastRotation);
    lastRotation = rotation;
    return {displacement, rotation, glm::vec3(1.0f)};
  };
}

glm::vec3 Buoy::calcGerstnerDisplacement(std::vector<std::shared_ptr<Wave>> waves, float time) {
  auto positionXZ = glm::vec2(getMoveable().position.x, getMoveable().position.z); 
  auto displacement = evaluateGerstnerWaves(waves, positionXZ, time);
  return displacement;
}
