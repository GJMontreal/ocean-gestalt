#pragma once

#include <CivetServer.h>
#include "api/ApiAdapter.hpp"

class PauseHandler : public CivetHandler {
public:
 static constexpr const char* uri() { return "/api/simulation/pause"; }
    PauseHandler(ApiAdapter& api) : api(api) {}

    bool handlePost(CivetServer* server, struct mg_connection* conn) override;

private:
    ApiAdapter& api;
};
