#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

using glm::radians;

Camera::Camera(vec3 position, vec3 up, float yaw, float pitch)
    : Front(vec3(0.0f, 0.0f, -1.0f)),
      
      MouseSensitivity(SENSITIVITY),
      Zoom(ZOOM) {
  movementSpeed = SPEED;
  position = position;
  moveUp = up;
  Yaw = yaw;
  Pitch = pitch;
  updateCameraVectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
mat4 Camera::GetViewMatrix() const {
  // if these values aren't changing, then how is the view changing?
  return glm::lookAt(position, position + Front, Up);
}

// processes input received from a mouse input system. Expects the offset value
// in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset,
                                  float yoffset,
                                  GLboolean constrainPitch) {
  xoffset *= MouseSensitivity;
  yoffset *= MouseSensitivity;
  Yaw += xoffset;
  Pitch += yoffset;

  // make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrainPitch) {
    if (Pitch > 89.0f)
      Pitch = 89.0f;
    if (Pitch < -89.0f)
      Pitch = -89.0f;
  }

  // update Front, Right and Up Vectors using the updated Euler angles
  updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input
// on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
  Zoom -= yoffset;
  if (Zoom < 1.0f)
    Zoom = 1.0f;
  if (Zoom > 45.0f)
    Zoom = 45.0f;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
  // calculate the new Front vector
  glm::vec3 front;
  front.x = cos(radians(Yaw)) * cos(radians(Pitch));
  front.y = sin(radians(Pitch));
  front.z = sin(radians(Yaw)) * cos(radians(Pitch));
  Front = glm::normalize(front);

  // also re-calculate the Right and Up vector
  Right = glm::normalize(glm::cross(
      Front, moveUp));  // normalize the vectors, because their length gets
                         // closer to 0 the more you look up or down which
                         // results in slower movement.
  Up = glm::normalize(glm::cross(Right, Front));

  glm::vec3 forward;
  forward.x = cos(radians(Yaw));
  forward.y = sin(radians(0.0f));
  forward.z = sin(radians(Yaw));

  moveForward = glm::normalize(forward);
  moveRight = glm::normalize(glm::cross(moveForward, moveUp));
}