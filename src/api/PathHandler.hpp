#pragma once

#include "UniformState.hpp"

struct ParsedUniformPath {
  std::optional<std::string> shaderName = std::nullopt;
  std::string internalPath;
  std::string externalPath;
  bool convertHeading = false;
};

struct PathHandler {
  virtual bool matches(const std::vector<std::string>& parts) const = 0;
  virtual std::optional<ApiValue> set(const std::vector<std::string>& parts, const ApiValue& value) = 0;
  virtual std::optional<ApiValue> get(const std::vector<std::string>& parts) = 0;
  virtual ~PathHandler() = default;
};

class UniformPathHandler: public PathHandler{
  public:
  UniformPathHandler(std::shared_ptr<UniformState> uniformState, std::vector<std::string> shaderNames) :
   uniformState(uniformState), shaderNames(shaderNames){};
  bool matches(const std::vector<std::string>& parts) const;

  std::optional<ApiValue> set(const std::vector<std::string>& parts, const ApiValue& value);
  std::optional<ApiValue> get(const std::vector<std::string>& parts);

  protected:
   ParsedUniformPath parseUniformPath(const std::vector<std::string>& parts);
  
   private:
  std::weak_ptr<UniformState> uniformState;
  std::vector<std::string> shaderNames;
};
