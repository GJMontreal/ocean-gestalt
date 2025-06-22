#pragma once

#include "Identifier.hpp"

#include <glm/glm.hpp>
#include <functional>


class Drawable;

using glm::vec3;

enum class Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Moveable;
class MoveableInterface {
public:
    virtual Moveable& getMoveable() = 0;
    virtual const Moveable& getMoveable() const = 0;
    virtual ~MoveableInterface() = default;
};

template <typename Derived>
class MoveableBase : public MoveableInterface {
public:
    Moveable& getMoveable() override { return static_cast<Derived&>(*this).moveable; }
    const Moveable& getMoveable() const override { return static_cast<const Derived&>(*this).moveable; }
};

class Moveable: public Identifier{
  public:
    void ProcessKeyboard(Movement direction, float deltaTime);
    float movementSpeed = 5.0f;

    vec3 position;
    std::function<void(glm::vec3 position)> onPositionChanged;     
    // these should be the camera's front up and right vectors
    vec3 moveForward{0.0,0.0,-1.0};
    vec3 moveUp{0.0,1.0,0.0};
    vec3 moveRight{1.0,0.0,0.0};

    std::string getName() const override {return "";};
    ~Moveable()override;
};
