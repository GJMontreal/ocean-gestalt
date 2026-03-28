#include "PathHandler.hpp"


using json = nlohmann::json;

static const int BUFFER_SIZE = 1024;

// ---- Pure logic helpers ----

std::string PathHandler::normalisePath(const std::string& rawPath) {
  const std::string prefix("/api/");
  std::string path = rawPath.substr(prefix.length());
  std::replace(path.begin(), path.end(), '/', '.');
  return path;
}

std::optional<PathHandler::PostRequest> PathHandler::parsePost(const std::string& body) {
  try {
    json request = json::parse(body);
    if (!request.contains("value") || !request.contains("path")) {
      return std::nullopt;
    }
    PostRequest req;
    req.path = request["path"].get<std::string>();
    req.value = request["value"];
    if (request.contains("duration") && request["duration"].is_number()) {
      req.duration = request["duration"].get<float>();
    }
    return req;
  } catch (const json::parse_error&) {
    return std::nullopt;
  }
}

// ---- CivetWeb adapters ----

void PathHandler::writeCORSHeaders(struct mg_connection* conn,
                                   const char* contentType) {
  const char* origin = mg_get_header(conn, "Origin");

  mg_printf(conn, "HTTP/1.1 200 OK\r\n");

  if (contentType) {
    mg_printf(conn, "Content-Type: %s\r\n", contentType);
  }

  if (origin) {
    mg_printf(conn, "Access-Control-Allow-Origin: %s\r\n", origin);
  } else {
    mg_printf(conn, "Access-Control-Allow-Origin: *\r\n");
  }

  mg_printf(conn,
            "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n"
            "Access-Control-Max-Age: 86400\r\n");
}

bool PathHandler::handleGet(CivetServer*, struct mg_connection* conn) {
  const struct mg_request_info* req_info = mg_get_request_info(conn);
  auto path = normalisePath(req_info->local_uri);
  auto result = handleGet(path);
  if (result) {
    writeCORSHeaders(conn);
    mg_printf(conn, "\r\n{\"value\": %s }\n", result->c_str());
    return true;
  }
  return false;
}

bool PathHandler::handlePost(CivetServer*, struct mg_connection* conn) {
  char buffer[BUFFER_SIZE];
  int len = mg_read(conn, buffer, sizeof(buffer) - 1);
  buffer[len] = '\0';

  auto req = parsePost(std::string(buffer, len));
  if (!req) {
    mg_printf(conn,
              "HTTP/1.1 400 Bad Request\r\nContent-Type: "
              "text/plain\r\n\r\nMissing or invalid json\n");
    return false;
  }

  auto result = handlePost(req->path, req->value, req->duration);
  if (result) {
    writeCORSHeaders(conn);
    mg_printf(conn, "\r\n{\"value\": %s }\n", result->c_str());
    return true;
  }
  return false;
}
