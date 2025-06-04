/**
 * Main.cpp skeleton
 * Contributors:
 *      * Arthur Sonzogni
 * Licence:
 *      * MIT
 */

#include "OceanGestalt.hpp"
#include "RestServer.hpp"

#include <memory>
// #include <GLFW/glfw3.h>

#include <memory>
int main(int argc, const char* argv[]) {
  auto app = std::make_shared<OceanGestalt>();
  auto server = std::make_unique<RestServer>();
  app->run();
  return 0;
}
