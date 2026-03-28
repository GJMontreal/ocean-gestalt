#include "RestServer.hpp"
#include "PauseHandler.hpp"
#include "UniformHandler.hpp"

#include <string>
#include <iostream>

RestServer::RestServer(std::shared_ptr<ApiAdapter> api,
                       std::shared_ptr<UniformAnimator> animator,
                       const std::string& portString)
    : api(std::move(api)), animator(std::move(animator)) {
  const char* options[] = {
        "listening_ports",portString.c_str(),
        nullptr
    };

  std::cout << "Starting api server at " << portString << std::endl;
  server = std::make_unique<CivetServer>(options);
  addHandlers();
}

void RestServer::addHandlers() {
  server->addHandler(PauseHandler::uri(), new PauseHandler(*api));
  auto uniformHandler = new UniformHandler(*api, animator);
  server->addHandler(uniformHandler->uri(), uniformHandler);
}
