#pragma once

#include "Moveable.hpp"
#include "Sphere.hpp"

class Light;
class Configuration;

#include <glm/glm.hpp>
#include "glm/detail/type_vec.hpp"

using glm::vec3;

class Light: public MoveableBase<Light>{
  public:
    explicit Light(glm::vec3 origin, std::shared_ptr<Configuration> context);

    inline std::shared_ptr<Configuration> getContext() {
    if(auto locked = this->context.lock()){
      return locked;
    }
    return nullptr;
    }

    inline void setOrigin(glm::vec3 origin) {};
    const inline glm::vec3 getPosition() const {return moveable.position;};

    const std::shared_ptr<Sphere> getDrawable() {return sphere;};

    void activate() override;
    void deactivate() override;
  private:
    std::weak_ptr<Configuration> context;
    std::shared_ptr<Sphere> sphere; // visual representation of the light source

    friend MoveableBase<Light>;
    Moveable moveable;
};

