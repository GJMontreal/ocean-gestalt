#pragma once

#include "Moveable.hpp"
#include "Configuration.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>

using glm::mat4;
using glm::vec3;

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera : public MoveableBase<Camera> {
 public:
  // updating these directly won't properly recalculate camera vectors
  // camera Attributes

  Camera(vec3 position = vec3(0.0f, 0.0f, 3.0f),
         vec3 up = vec3(0.0f, 1.0f, 0.0f),
         float yaw = YAW,
         float pitch = PITCH,
         float zoom = ZOOM);

  ~Camera() override = default;

  glm::vec3 getPosition() const { return moveable.position; };
  float getPitch() const { return Pitch;};
  float getYaw() const { return Yaw;};
  float getZoom() const { return Zoom;};

  void updateCameraVectors();
  void processMouseScroll(float yoffset);
  void ProcessMouseMovement(float xoffset,
                            float yoffset,
                            GLboolean constrainPitch = true);
  mat4 getViewMatrix(double time) const;

  const MoveDirection& getMoveDirection() const { return moveDirection; };
  MoveDirection& getMoveDirection() { return this->moveDirection; };

  void activate() override{/* do nothing */};
  void deactivate() override{/* do nothing */};

  void setConfiguration(std::shared_ptr<Configuration> configuration) {context = configuration;};

  void setIsFloating(bool floating) { isFloating = floating; };
  bool getIsFloating() const { return isFloating; };

 private:
  friend class MoveableBase<Camera>;
  Moveable moveable;
  
  std::weak_ptr<Configuration> context;

  vec3 Front;
  vec3 Up;
  vec3 Right;

  // euler Angles
  float Yaw;
  float Pitch;
  float Zoom;
  MoveDirection moveDirection;

  // camera options
  float MouseSensitivity;

  // simulation
  bool isFloating;
};
