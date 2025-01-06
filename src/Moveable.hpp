#ifndef __MOVEABLE_HPP
#define __MOVEABLE_HPP

#include <glm/glm.hpp>

using glm::vec3;

enum class Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Moveable{
  public:
    virtual void ProcessKeyboard(Movement direction, float deltaTime) = 0;
    float movementSpeed = 5.0f;

    vec3 forward{0.0,0.0,-1.0};
    vec3 up{0.0,1.0,0.0};
    vec3 right{1.0,0.0,0.0};

  protected:
    virtual ~Moveable() = default;
};

#endif
