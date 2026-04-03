#pragma once

#include "Identifier.hpp"

#include <glm/glm.hpp>
#include <functional>
#include <optional>


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

struct MoveDirection{
  glm::vec3 up;
  glm::vec3 forward;
  glm::vec3 right;
};

class Moveable;
class MoveableInterface {
public:
    virtual Moveable& getMoveable() = 0;
    virtual const Moveable& getMoveable() const = 0;

    virtual void activate() = 0;
    virtual void deactivate() = 0;

    virtual ~MoveableInterface() = default;
};

template <typename Derived>
class MoveableBase : public MoveableInterface {
public:
    Moveable& getMoveable() override { return static_cast<Derived&>(*this).moveable; }
    const Moveable& getMoveable() const override { return static_cast<const Derived&>(*this).moveable; }
    void activate() override { static_cast<Derived&>(*this).activate();};  //implement your own if you'd like 
    void deactivate() override { static_cast<Derived&>(*this).deactivate();};  //implement your own if you'd like

};

class Moveable {
  public:
    void ProcessKeyboard(Movement direction, float deltaTime);
    float movementSpeed = 5.0f;
    void setPosition(glm::vec3 aPosition) {
      this->position = aPosition;
      if (onPositionChanged) {
        onPositionChanged(aPosition);
      };
    };
    vec3 position;
    std::function<void(glm::vec3 position)> onPositionChanged;  
    
    std::function<MoveDirection&()> getMoveDirection;

    void setIsFloating(bool floating) { isFloating = floating; };
    bool getIsFloating() const { return isFloating; };
protected:

    // simulation
    bool isFloating = true;
};
