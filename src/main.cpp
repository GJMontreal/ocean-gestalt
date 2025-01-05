/**
 * Main.cpp skeleton
 * Contributors:
 *      * Arthur Sonzogni
 * Licence:
 *      * MIT
 */

#include "MyApplication.hpp"

#include <GLFW/glfw3.h>

#include <memory>
int main(int argc, const char* argv[]) {
  auto app = MyApplication();
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glfwSetInputMode(app.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  app.run();
  return 0;
}
