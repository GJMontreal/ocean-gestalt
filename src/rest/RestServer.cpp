#include "RestServer.hpp"
#include <string>

RestServer::RestServer(int port) {
  std::string portString = std::to_string((port));
  const char* options[] = {
        "listening_ports",portString.c_str(),
        nullptr
    };
  server = std::make_unique<CivetServer>(options);
}

void RestServer::addHandlers() {

}
