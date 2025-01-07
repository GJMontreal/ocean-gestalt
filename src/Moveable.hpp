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
    void ProcessKeyboard(Movement direction, float deltaTime);
    float movementSpeed = 5.0f;

    vec3 position;
    
    vec3 moveForward{0.0,0.0,-1.0};
    vec3 moveUp{0.0,1.0,0.0};
    vec3 moveRight{1.0,0.0,0.0};

  protected:
    virtual ~Moveable() = default;
};

#endif
