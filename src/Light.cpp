#include "Light.hpp"

Light::Light(vec3 aPosition){
  position = aPosition;
}

// This could move into the base class once we rename some stuff in the camera
void Light::ProcessKeyboard(Movement direction, float deltaTime) {
  float velocity = movementSpeed * deltaTime;
  if (direction == Movement::FORWARD)
    position += forward * velocity;
  if (direction == Movement::BACKWARD)
    position -= forward * velocity;
  if (direction == Movement::LEFT)
    position -= right * velocity;
  if (direction == Movement::RIGHT)
    position += right * velocity;
  if (direction == Movement::UP)
    position += up * velocity;
  if (direction == Movement::DOWN)
    position -= up * velocity;
}