#ifndef __REST_SERVER_HPP
#define __REST_SERVER_HPP

#include <memory>

#include <CivetServer.h>

class RestServer{
  public:
  RestServer(int port = 8080);
  ~RestServer() = default;

  void addHandlers();

private:
  std::unique_ptr<CivetServer> server;
};



#endif
