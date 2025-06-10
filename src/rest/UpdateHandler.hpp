#pragma once

#include <CivetServer.h>
#include "api/ApiAdapter.hpp"

class UpdateHandler : public CivetHandler {
 public:
  static constexpr const char* uri() { return "/update"; }

  UpdateHandler(ApiAdapter& api) : api(api) {}

  bool handlePost(CivetServer* server, struct mg_connection* conn) override;

 private:
  ApiAdapter& api;
};
