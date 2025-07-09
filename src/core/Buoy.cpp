#include "Buoy.hpp"

#include "Camera.hpp"
#include "Configuration.hpp"
#include "Moveable.hpp"
#include "Sphere.hpp"
#include "GerstnerWave.hpp"

Buoy::Buoy(glm::vec3 origin, std::shared_ptr<Configuration> context) : context(context){

  sphere = std::make_shared<Sphere>(origin, glm::vec4(0.85f,0.31f,0.f,1.f), context,2.0f);
  
  moveable.onPositionChanged = [&](const glm::vec3& pos) {
    this->sphere->setOrigin(pos);
  };

  moveable.setPosition(origin);  //important we don't use getMoveable here as it will refer to the base class
  moveable.getMoveDirection = [this]()->MoveDirection&{
    return this->getContext()->camera->getMoveDirection(); //capturing context directly fails in here. It must be changing
  };

  sphere->preDraw = [this](Drawable& drawable, float time)->glm::mat4{
  auto offset = calcGerstnerDisplacement(this->getContext()->getWaves(), time);
  auto transform = glm::translate(glm::mat4(1.f),offset);
  return transform;
  };
}

glm::vec3 Buoy::calcGerstnerDisplacement(std::vector<std::shared_ptr<Wave>> waves, float time) {
  auto positionXZ = glm::vec2(getMoveable().position.x, getMoveable().position.z); 
  auto displacement = evaluateGerstnerWaves(waves, positionXZ, time);
  return displacement;
}
