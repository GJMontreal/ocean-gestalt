#include "KeyExecutable.hpp"

void KeyExecutable::executeIfPressed(GLFWwindow* window,
                           int key,
                           function<void()>const &lambda) {
  if (glfwGetKey(window, key) == GLFW_PRESS) {
    if (keyPressState[(char)key] == GLFW_RELEASE) {
      lambda();
      keyPressState[(char)key] = GLFW_PRESS;
    }
  } else {
    keyPressState[(char)key] = GLFW_RELEASE;
  }
}
