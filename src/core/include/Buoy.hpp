#pragma once

#include "Moveable.hpp"
#include <glm/gtc/quaternion.hpp>

class Configuration;
class Sphere;
class Wave;

class Buoy: public MoveableBase<Buoy> {
  public:
    Buoy(glm::vec3 origin, std::shared_ptr<Configuration> context);
    std::shared_ptr<Sphere> getDrawable() {return sphere;};

    std::shared_ptr<Configuration> getContext() {return context.lock();}; 

    void activate() override { /* disconnect from simulation */};
    void deactivate() override { /* reconnect to simulation */};

    glm::vec3 calcGerstnerDisplacement(std::vector<std::shared_ptr<Wave>> waves, float time);
    
    private:
    friend MoveableBase<Buoy>;
    Moveable moveable;
    
    glm::quat lastRotation;

    std::shared_ptr<Sphere> sphere;
    std::weak_ptr<Configuration> context;
};

