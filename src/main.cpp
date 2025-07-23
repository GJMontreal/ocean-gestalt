/**
 * Main.cpp skeleton
 * Contributors:
 *      * Arthur Sonzogni
 * Licence:
 *      * MIT
 */

#include "OceanGestalt.hpp"

#ifndef __EMSCRIPTEN__
#include "RestServer.hpp"
#else
#include <emscripten/emscripten.h>
#endif

#include "OceanApi.hpp"
#include "UniformState.hpp"

#include <memory>
#include <iostream>

std::shared_ptr<OceanGestalt> app;

int main(int argc, const char* argv[]) {
  app = std::make_shared<OceanGestalt>();

  auto uniformState = std::make_shared<UniformState>(app->getContext());
  auto api = std::make_shared<OceanApi>(app,uniformState);
  app->onRender([&uniformState](float){uniformState->renderThreadCallback();});
  app->doOnReady([&] {
      app->getContext().setInitialUniformState(*api);
  });

  app->getContext().setApi(api);
  #ifndef __EMSCRIPTEN__
    auto server = std::make_unique<RestServer>(api,"0.0.0.0:8080");
    server->addHandlers(); // why does this have to explicit 
  #endif
  app->run();
  return 0;
}

#ifdef __EMSCRIPTEN__
extern "C" {
  EMSCRIPTEN_KEEPALIVE
  void onPageHidden() {
    app->muteAudio(true);
  }

  EMSCRIPTEN_KEEPALIVE
  void onPageVisible() {
    if(app->getIsRunning()){
      app->muteAudio(false);
    }
  }
}
#endif