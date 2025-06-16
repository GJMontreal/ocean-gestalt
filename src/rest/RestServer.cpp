#include "RestServer.hpp"
#include "PauseHandler.hpp"
#include "UniformHandler.hpp"

#include <string>
#include <iostream>

RestServer::RestServer(std::shared_ptr<ApiAdapter>&& api, const std::string& portString)
    : api(std::move(api)) {
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
  auto uniformHandler = new UniformHandler(*api);
  server->addHandler(uniformHandler->uri(), uniformHandler);
}
