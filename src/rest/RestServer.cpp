#include "RestServer.hpp"
#include "PauseHandler.hpp"

#include <string>

RestServer::RestServer(std::unique_ptr<ApiAdapter>&& api, int port)
    : api(std::move(api)) {
  std::string portString = std::to_string((port));
  const char* options[] = {
        "listening_ports",portString.c_str(),
        nullptr
    };
  server = std::make_unique<CivetServer>(options);
}

void RestServer::addHandlers() {
  server->addHandler("/pause", new PauseHandler(*api));
}
