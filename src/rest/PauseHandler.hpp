#pragma once

#pragma once

#include <CivetServer.h>
#include "ApiAdapter.hpp"

class PauseHandler : public CivetHandler {
public:
    PauseHandler(ApiAdapter& api) : api(api) {}

    bool handlePost(CivetServer* server, struct mg_connection* conn) override;

private:
    ApiAdapter& api;
};
