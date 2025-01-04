#ifndef __INPUT_PROCESSOR_HPP
#define __INPUT_PROCESSOR_HPP

#include <GLFW/glfw3.h>

#include <map>

using std::map;

class InputProcessor{
  public:
    virtual ~InputProcessor() = default;
    virtual void processInput(GLFWwindow *window, float deltaTime) = 0;

    map<int, int> keyPressState;
};

class Updatable{
  public:
  virtual ~Updatable() = default;
  virtual void update() = 0;
};

#endif
