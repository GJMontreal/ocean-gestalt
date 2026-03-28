#pragma once

#include "UniformState.hpp"
#include <cmath>

class buoy;

inline float vecToHeading(float x, float y) {
    float rad = std::atan2(y, x);
    float deg = rad * 180.0f / M_PI;
    return (deg < 0.0f) ? deg + 360.0f : deg;
}

inline std::vector<float> headingToVec3(float deg) {
    float rad = deg * M_PI / 180.0f;
    return {std::cos(rad), std::sin(rad), 0.0f};
}

struct ParsedUniformPath {
  std::optional<std::string> shaderName = std::nullopt;
  std::string internalPath;
  std::string externalPath;
  bool convertHeading = false;
  bool isWaveRelated = false; //rather why don't we have something like simulationPath
  std::optional<std::string> simulationPath = std::nullopt;
};

struct PathHandler {
  virtual bool matches(const std::vector<std::string>& parts) const = 0;
  virtual std::optional<ApiValue> set(const std::vector<std::string>& parts, const ApiValue& value, bool block) = 0;
  virtual std::optional<ApiValue> get(const std::vector<std::string>& parts) = 0;
  virtual ~PathHandler() = default;
};

class UniformPathHandler: public PathHandler{
  public:
  UniformPathHandler(std::shared_ptr<UniformState> uniformState, std::vector<std::string> shaderNames) :
   uniformState(uniformState), shaderNames(shaderNames){};
  bool matches(const std::vector<std::string>& parts) const;

  std::optional<ApiValue> set(const std::vector<std::string>& parts, const ApiValue& value, bool block);
  std::optional<ApiValue> get(const std::vector<std::string>& parts);

  std::function<void(const std::string&, const ApiValue&)> syncWaveFunction;
  
  protected:
   ParsedUniformPath parseUniformPath(const std::vector<std::string>& parts);
 
   private:
  std::weak_ptr<UniformState> uniformState;
  std::vector<std::string> shaderNames;


};
