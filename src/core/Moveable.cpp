#include "Moveable.hpp"

// This could move into the base class once we rename some stuff in the camera
void Moveable::ProcessKeyboard(Movement direction, float deltaTime) {
  MoveDirection moveDirection;
  if(getMoveDirection){
    moveDirection = getMoveDirection();
  }
  float velocity = movementSpeed * deltaTime;
  switch (direction) {
    case Movement::FORWARD:
      position += moveDirection.forward * velocity;
      break;
    case Movement::BACKWARD:
      position -= moveDirection.forward * velocity;
      break;
    case Movement::LEFT:
      position -= moveDirection.right * velocity;
      break;
    case Movement::RIGHT:
      position += moveDirection.right * velocity;
      break;
    case Movement::UP:
      position += moveDirection.up * velocity;
      break;
    case Movement::DOWN:
      position -= moveDirection.up * velocity;
  }
  
  if(onPositionChanged){
    onPositionChanged(position);
  }
}
