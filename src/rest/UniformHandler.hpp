#pragma once

#include "PathHandler.hpp"

using SetUniformFunc = std::function<std::optional<std::string>(
    const std::string&, const std::string&, const nlohmann::json::value_type&)>;

struct JsonTypeHandler {
  std::function<bool(const nlohmann::json::value_type&)> match;
  SetUniformFunc apply;
};
class UniformHandler : public PathHandler {
 public:
  // UniformHandler(ApiAdapter& api) : PathHandler(api) {}
  UniformHandler(ApiAdapter& api);
  const char* uri() const override { return "/api/uniforms/*"; };

 protected:
  std::optional<std::string> handleGet(std::string path) override;
  std::optional<std::string> handlePost(std::string path,
                                        nlohmann::json::value_type) override;

  private:
    std::vector<JsonTypeHandler> handlers;

};
