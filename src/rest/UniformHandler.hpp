#pragma once

#include "PathHandler.hpp"
#include "ApiAdapter.hpp"
#include <memory>

class UniformAnimator;

using ApplyFunc = std::function<ApiValue(nlohmann::json::value_type&)>;
using MatchFunc = std::function<bool(const nlohmann::json::value_type&)>;

struct JsonTypeHandler {
  MatchFunc match;
  ApplyFunc apply;
  std::string handlerName;  //purely for simplifying debugging
};

struct UniformKey{
  std::string shaderName;
  std::string uniformName;
};

class UniformHandler : public PathHandler {
 public:

  UniformHandler(ApiAdapter& api, std::shared_ptr<UniformAnimator> animator);
  const char* uri() const override { return "/api/uniforms/*"; };

 protected:
  std::optional<std::string> handleGet(const std::string& path) override;
  std::optional<std::string> handlePost(const std::string& path,
                                        nlohmann::json::value_type,
                                        std::optional<float> duration) override;

  // utilities
  std::optional<UniformKey> splitPath(const std::string_view& path);

  private:
    std::shared_ptr<UniformAnimator> animator;
    std::vector<JsonTypeHandler> handlers;
};
