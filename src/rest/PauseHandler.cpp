#include "PauseHandler.hpp"

#include "PauseHandler.hpp"
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

bool PauseHandler::handlePost(CivetServer*, struct mg_connection* conn) {
    char buffer[1024];
    int len = mg_read(conn, buffer, sizeof(buffer) - 1);
    buffer[len] = '\0';

    try {
        json request = json::parse(buffer);

        if (!request.contains("pause") || !request["pause"].is_boolean()) {
            mg_printf(conn,
                "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nMissing or invalid 'pause' field\n");
            return true;
        }

        bool pause = request["pause"];
        api.pauseSimulation(pause);

        json response = {
            {"status", "ok"},
            {"paused", pause}
        };

        std::string body = response.dump();

        mg_printf(conn,
            "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %lu\r\n\r\n%s",
            body.size(), body.c_str());
        return true;

    } catch (const json::parse_error& e) {
        mg_printf(conn,
            "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid JSON\n");
        return true;
    }
}
