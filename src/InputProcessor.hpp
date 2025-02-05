#ifndef __INPUT_PROCESSOR_HPP
#define __INPUT_PROCESSOR_HPP

#include <GLFW/glfw3.h>
#include <memory>

class InputProcessor{
  public:
    virtual ~InputProcessor() = default;
    virtual void processInput(GLFWwindow *window, float deltaTime) = 0;
};

class Updatable: public std::enable_shared_from_this<Updatable>{
  public:
  virtual ~Updatable() = default;
  bool wavesNeedUpdate = true;
};

#endif
