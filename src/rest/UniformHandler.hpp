#pragma once

#include "PathHandler.hpp"
#include "ApiAdapter.hpp"

using SetUniformFunc = std::function<std::optional<UniformValue>(
    const std::string&, const std::string&, const nlohmann::json::value_type&)>;

using ReturnFunc = std::function<std::optional<nlohmann::json> (const std::any&)>;

struct JsonTypeHandler {
  std::function<bool(const nlohmann::json::value_type&)> match;
  SetUniformFunc apply;
  std::string handlerName;  //purely for simplifying debugging
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
    std::optional<UniformKey> splitPath(const std::string_view& path);
};
