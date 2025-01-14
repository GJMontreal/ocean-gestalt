/**
 * Main.cpp skeleton
 * Contributors:
 *      * Arthur Sonzogni
 * Licence:
 *      * MIT
 */

#include  "OceanGestalt.hpp"

#include <GLFW/glfw3.h>

#include <memory>
int main(int argc, const char* argv[]) {
  auto app = OceanGestalt();
  app.run();
  return 0;
}
