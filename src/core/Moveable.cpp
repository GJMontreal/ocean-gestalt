#include "Moveable.hpp"

#include <iostream>
#include <glm/gtx/string_cast.hpp>

// This could move into the base class once we rename some stuff in the camera
void Moveable::ProcessKeyboard(Movement direction, float deltaTime) {
  MoveDirection moveDirection;
  if(getMoveDirection){
    moveDirection = getMoveDirection();
  }
  float velocity = movementSpeed * deltaTime;
  if (direction == Movement::FORWARD)
    position += moveDirection.forward * velocity;
  if (direction == Movement::BACKWARD)
    position -= moveDirection.forward * velocity;
  if (direction == Movement::LEFT){
    std::cout << "left" << std::endl;
    position -= moveDirection.right * velocity;
  }
  if (direction == Movement::RIGHT){
    std::cout << "right" <<std::endl;
    position += moveDirection.right * velocity;
  }
  if (direction == Movement::UP)
    position += moveDirection.up * velocity;
  if (direction == Movement::DOWN)
    position -= moveDirection.up * velocity;

  std::cout << glm::to_string(position) << std::endl;
  if(onPositionChanged){
    onPositionChanged(position);
  }
}
