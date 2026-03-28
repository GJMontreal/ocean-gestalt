#include "UpdateHandler.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

static const int BUFFER_SIZE = 1024;

bool UpdateHandler::handlePost(CivetServer*, struct mg_connection* conn) {
    char buffer[BUFFER_SIZE];
    int len = mg_read(conn, buffer, sizeof(buffer) - 1);
    buffer[len] = '\0';

    try {
        json request = json::parse(buffer);

        // our request must contain
        if (!request.contains("path")  || !request.contains("value") 
        || !request["path"].is_string() || !request["value"].is_string()) {
            mg_printf(conn,
                "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nMissing or invalid payload\n");
            return true;
        }

        json response = {
            {"status", "ok"}
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