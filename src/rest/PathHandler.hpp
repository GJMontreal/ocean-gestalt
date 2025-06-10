#pragma once

#include <CivetServer.h>
#include "ApiAdapter.hpp"

#include "nlohmann/json.hpp"

#include <optional>
#include <string>

using json = nlohmann::json;
class PathHandler : public CivetHandler {
 public:

  virtual const char* uri() const = 0;
  PathHandler(ApiAdapter& api) : api(api) {}

 protected:
  bool handleGet(CivetServer*, struct mg_connection* conn) override;
  virtual std::optional<std::string> handleGet(const std::string& path) = 0;

  bool handlePost(CivetServer*, struct mg_connection* conn) override;
  virtual std::optional<std::string> handlePost(
      const std::string& path,
      nlohmann::json::value_type value) = 0;

  std::string getSubPath(const struct mg_request_info* req_info) const;

  void writeCORSHeaders(struct mg_connection* conn,
                        const char* contentType = "application/json");

  ApiAdapter& api;
};
