/**
 * Main.cpp skeleton
 * Contributors:
 *      * Arthur Sonzogni
 * Licence:
 *      * MIT
 */

#include "OceanGestalt.hpp"
#include "RestServer.hpp"
#include "OceanApi.hpp"

#include <memory>

int main(int argc, const char* argv[]) {
  auto app = std::make_shared<OceanGestalt>();
  auto api = std::make_unique<OceanApi>(*app);
  // app->doOnReady([&api]{api->setupShaderNormalInterface();});
  auto server = std::make_unique<RestServer>(std::move(api));
  server->addHandlers(); // why does this have to explicit
  app->run();
  return 0;
}
