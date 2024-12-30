#ifndef __CAMERA_HPP
#define __CAMERA_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

enum class Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera{
public:
  //updating these directly might not properly recalculate camera Vectors
  // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;

    glm::vec3 WorldUp;
    glm::vec3 DollyRight;
    glm::vec3 DollyFront;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW,
           float pitch = PITCH);

//TO DO: make capitalization consistent
    void updateCameraVectors();
    void ProcessMouseScroll(float yoffset);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    glm::mat4 GetViewMatrix() const;
};

#endif
