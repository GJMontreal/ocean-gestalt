#include "Moveable.hpp"

// This could move into the base class once we rename some stuff in the camera
void Moveable::ProcessKeyboard(Movement direction, float deltaTime) {
  float velocity = movementSpeed * deltaTime;
  if (direction == Movement::FORWARD)
    position += moveForward * velocity;
  if (direction == Movement::BACKWARD)
    position -= moveForward * velocity;
  if (direction == Movement::LEFT)
    position -= moveRight * velocity;
  if (direction == Movement::RIGHT)
    position += moveRight * velocity;
  if (direction == Movement::UP)
    position += moveUp * velocity;
  if (direction == Movement::DOWN)
    position -= moveUp * velocity;
}