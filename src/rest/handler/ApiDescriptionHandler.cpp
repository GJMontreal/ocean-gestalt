#include "ApiDescriptionHandler.hpp"

#include <fstream>
#include <sstream>
#include <civetweb.h>

class ApiDescriptionHandler : public CivetHandler {
public:
    bool handleGet(CivetServer *server, struct mg_connection *conn) override {
        std::ifstream file("params.json");
        if (!file) {
            mg_printf(conn,
                "HTTP/1.1 500 Internal Server Error\r\n"
                "Content-Type: text/plain\r\n\r\n"
                "Failed to open params.json\n");
            return true;
        }

        std::ostringstream contents;
        contents << file.rdbuf();

        mg_printf(conn,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %zu\r\n\r\n%s",
            contents.str().size(),
            contents.str().c_str());

        return true;
    }
};
              