#ifndef __CAMERA_HPP
#define __CAMERA_HPP

#include "Moveable.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

using glm::vec3;
using glm::mat4;

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  5.0f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

class Camera: public Moveable{
public:
   //updating these directly won't properly recalculate camera vectors
  // camera Attributes
    vec3 Front;
    vec3 Up;
    vec3 Right;

    // euler Angles
    float Yaw;
    float Pitch;

    // camera options
    float MouseSensitivity;
    float Zoom;

  Camera(vec3 position = vec3(0.0f, 0.0f, 3.0f),
           vec3 up = vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW,
           float pitch = PITCH);
        
  ~Camera() override = default;

// TODO:  make capitalization consistent
    void updateCameraVectors();
    void ProcessMouseScroll(float yoffset);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    mat4 GetViewMatrix() const;
};

#endif
