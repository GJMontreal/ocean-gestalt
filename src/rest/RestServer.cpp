#include "RestServer.hpp"
#include "PauseHandler.hpp"
#include "UniformHandler.hpp"

#include <string>
#include <iostream>

RestServer::RestServer(std::unique_ptr<ApiAdapter>&& api, int port)
    : api(std::move(api)) {
  std::string portString = std::to_string((port));
  const char* options[] = {
        "listening_ports",portString.c_str(),
        nullptr
    };

  std::cout << "Starting api server at " << port << std::endl;
  server = std::make_unique<CivetServer>(options);
}

void RestServer::addHandlers() {
  server->addHandler(PauseHandler::uri(), new PauseHandler(*api));
  auto uniformHandler = new UniformHandler(*api);
  server->addHandler(uniformHandler->uri(), uniformHandler);
}
