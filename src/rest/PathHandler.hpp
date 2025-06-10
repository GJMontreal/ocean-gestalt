#pragma once

#include <CivetServer.h>
#include "api/ApiAdapter.hpp"

#include <optional>
#include <string>

class PathHandler : public CivetHandler {
 public:
  // constexpr const char* uri()
  virtual const char* uri() const = 0;
  PathHandler(ApiAdapter& api) : api(api) {}

 protected:
  bool handleGet(CivetServer*, struct mg_connection* conn) override;
  virtual std::optional<std::string> handleGet(std::string path)=0;
 
  bool handlePost(CivetServer*, struct mg_connection* conn) override;
  virtual std::optional<std::string> handlePost(std::string path, std::string value)=0;

  std::string getSubPath(const struct mg_request_info* req_info) const;

  void writeCORSHeaders(struct mg_connection* conn, const char* contentType = "application/json");

  ApiAdapter& api;
};
