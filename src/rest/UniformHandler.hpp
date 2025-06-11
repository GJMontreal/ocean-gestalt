#pragma once

#include "PathHandler.hpp"

using SetUniformFunc = std::function<std::optional<std::any>(
    const std::string&, const std::string&, const nlohmann::json::value_type&)>;

struct JsonTypeHandler {
  std::function<bool(const nlohmann::json::value_type&)> match;
  SetUniformFunc apply;
  std::string handlerName;
};

struct UniformKey{
  std::string shaderName;
  std::string uniformName;
};
class UniformHandler : public PathHandler {
 public:
 
  UniformHandler(ApiAdapter& api);
  const char* uri() const override { return "/api/uniforms/*"; };

 protected:
  std::optional<std::string> handleGet(const std::string& path) override;
  std::optional<std::string> handlePost(const std::string& path,
                                        nlohmann::json::value_type) override;

  private:
    std::vector<JsonTypeHandler> handlers;

    // utilities
    std::optional<nlohmann::json> anyToJson(const std::any& a);
    std::optional<UniformKey> splitPath(const std::string_view& path);
};
