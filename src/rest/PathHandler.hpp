#pragma once

#include <CivetServer.h>
#include "ApiAdapter.hpp"

#include "nlohmann/json.hpp"

#include <optional>
#include <string>

using json = nlohmann::json;

class PathHandler : public CivetHandler {
 public:

  struct PostRequest {
    std::string path;
    nlohmann::json value;
    std::optional<float> duration;
  };

  virtual const char* uri() const = 0;
  PathHandler(ApiAdapter& api) : api(api) {}

  // Testable pure-logic helpers (no mg_connection dependency)
  static std::optional<PostRequest> parsePost(const std::string& body);
  static std::string normalisePath(const std::string& rawPath);

 protected:
  bool handleGet(CivetServer*, struct mg_connection* conn) override;
  virtual std::optional<std::string> handleGet(const std::string& path) = 0;

  bool handlePost(CivetServer*, struct mg_connection* conn) override;
  virtual std::optional<std::string> handlePost(
      const std::string& path,
      nlohmann::json::value_type value,
      std::optional<float> duration = std::nullopt) = 0;

  void writeCORSHeaders(struct mg_connection* conn,
                        const char* contentType = "application/json");

  ApiAdapter& api;
};
