#ifndef __KEY_EXECUTABLE_HPP
#define __KEY_EXECUTABLE_HPP

#include <functional>
#include <map>
#include <GLFW/glfw3.h>

using std::function;
using std::map;

class KeyExecutable{
  public:
  KeyExecutable() = default;
  ~KeyExecutable() = default;


  void executeIfPressed(GLFWwindow* window, int key,function<void()> const& lambda );

  private:
    map<int, int> keyPressState;
};

#endif
