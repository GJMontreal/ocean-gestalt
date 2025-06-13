#pragma once

#include "UniformState.hpp"

struct PathHandler {
  virtual bool matches(const std::vector<std::string>& parts) const = 0;
  virtual std::optional<ApiValue> set(const std::vector<std::string>& parts, const ApiValue& value) = 0;
  virtual std::optional<ApiValue> get(const std::vector<std::string>& parts) = 0;
  virtual ~PathHandler() = default;
};

class UniformPathHandler: public PathHandler{
  public:
  UniformPathHandler(UniformState& uniformState) : uniformState(uniformState){};
  bool matches(const std::vector<std::string>& parts) const;
  std::optional<ApiValue> set(const std::vector<std::string>& parts, const ApiValue& value);
  std::optional<ApiValue> get(const std::vector<std::string>& parts);
  private:
  UniformState& uniformState;
};
