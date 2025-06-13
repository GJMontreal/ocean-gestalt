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
#include "UniformState.hpp"

#include <memory>

int main(int argc, const char* argv[]) {
  auto app = std::make_shared<OceanGestalt>();
  auto uniformState = std::make_shared<UniformState>(app->getContext());
  auto api = std::make_shared<OceanApi>(*app,*uniformState);

  app->onRender([&uniformState]{uniformState->renderThreadCallback();}); // who will own the context
  auto server = std::make_unique<RestServer>(api);
  server->addHandlers(); // why does this have to explicit
  app->run();
  return 0;
}
