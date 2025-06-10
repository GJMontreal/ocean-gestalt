#pragma once

#include "PathHandler.hpp"

class UniformHandler : public PathHandler {
 public:
  UniformHandler(ApiAdapter& api) : PathHandler(api) {}
  const char* uri() const override { return "/api/uniforms/*"; } ;
 
protected:
  std::optional<std::string> handleGet(std::string path) override;
  bool handlePost(std::string path, std::string value) override;
};
