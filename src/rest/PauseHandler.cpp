#include "PauseHandler.hpp"

#include "PauseHandler.hpp"
#include <string>
#include <sstream>

bool PauseHandler::handlePost(CivetServer*, struct mg_connection* conn) {
    char buffer[1024];
    int len = mg_read(conn, buffer, sizeof(buffer) - 1);
    buffer[len] = '\0';

    std::string body(buffer);
    //we should use a more reliable json parser here
    bool pause = body.find("pause=true") != std::string::npos;

    api.pauseSimulation(pause);

    mg_printf(conn,
        "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nSimulation paused = %s\n",
        pause ? "true" : "false"
    );
    return true;
}
