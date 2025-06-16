#ifndef __REST_SERVER_HPP
#define __REST_SERVER_HPP

#include <memory>

#include <CivetServer.h>
#include "ApiAdapter.hpp"

class RestServer{
  public:
  RestServer(std::shared_ptr<ApiAdapter>&& api, const std::string& portString = "8080");
  ~RestServer() = default;

  void addHandlers();

private:
  std::unique_ptr<CivetServer> server;
  std::shared_ptr<ApiAdapter> api;
};



#endif
