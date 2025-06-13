#include "PathHandler.hpp"

using json = nlohmann::json;

static const int BUFFER_SIZE = 1024;

void PathHandler::writeCORSHeaders(struct mg_connection* conn,
                                   const char* contentType) {
  const struct mg_request_info* req_info = mg_get_request_info(conn);
  const char* origin = mg_get_header(conn, "Origin");

  mg_printf(conn, "HTTP/1.1 200 OK\r\n");

  if (contentType) {
    mg_printf(conn, "Content-Type: %s\r\n", contentType);
  }

  if (origin) {
    mg_printf(conn, "Access-Control-Allow-Origin: %s\r\n", origin);
  } else {
    mg_printf(conn, "Access-Control-Allow-Origin: *\r\n");  // fallback
  }

  // Optional caching and security headers
  mg_printf(conn,
            "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n"
            "Access-Control-Max-Age: 86400\r\n");
};

std::string PathHandler::getSubPath(
    const struct mg_request_info* req_info) const {
  std::string fullPath = req_info->local_uri;

  const std::string prefix(uri());
  std::string subPath = fullPath.substr(prefix.length() - 1);
  return subPath;
}

bool PathHandler::handleGet(CivetServer* server, struct mg_connection* conn) {
  const struct mg_request_info* req_info = mg_get_request_info(conn);

  // TODO: use our refactored subfunction
  auto subPath = getSubPath(req_info);
  auto result = handleGet(subPath);
  if (result) {
    std::string value = *result;
    writeCORSHeaders(conn);
    mg_printf(conn,
              "\r\n"
              "{\"value\": %s }\n",
              value.c_str());
    return true;
  }
  return false;
}

bool PathHandler::handlePost(CivetServer* server, struct mg_connection* conn) {
  const struct mg_request_info* req_info = mg_get_request_info(conn);
  char buffer[BUFFER_SIZE];
  int len = mg_read(conn, buffer, sizeof(buffer) - 1);
  buffer[len] = '\0';

  auto subPath = getSubPath(req_info);

  try {
    json request = json::parse(buffer);

    // we should probably verify that "value" is a valid type as well
    if (!request.contains("value") || !request.contains("path")) {
      mg_printf(conn,
                "HTTP/1.1 400 Bad Request\r\nContent-Type: "
                "text/plain\r\n\r\nMissing or invalid json\n");
      return false;
    }

    auto value = request["value"];
    auto path = request["path"];
    
    auto result = handlePost(path, value);
    if (result) {
      std::string returnValue = *result;
      writeCORSHeaders(conn);
      mg_printf(conn,
                "\r\n"
                "{\"value\": %s }\n",
                returnValue.c_str());
      return true;
    } else {
      // error case so returnBadRequest(errorString);
      return false;
    }
  } catch (const json::parse_error& e) {
    mg_printf(conn,
              "HTTP/1.1 400 Bad Request\r\nContent-Type: "
              "text/plain\r\n\r\nInvalid JSON\n");
  }
  return false;
}
