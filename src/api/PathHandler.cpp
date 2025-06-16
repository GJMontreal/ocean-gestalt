#include "PathHandler.hpp"
#include "AppContextInterface.hpp"


float vecToHeading(float x, float y) {
  float rad = std::atan2(y, x);
  float deg = rad * 180.0f / M_PI;
  return (deg < 0.0f) ? deg + 360.0f : deg;
}

std::vector<float> headingToVec3(float deg) {
  float rad = deg * M_PI / 180.0f;
  return {std::cos(rad), std::sin(rad), 0.0f};
}


bool UniformPathHandler::matches(const std::vector<std::string>& parts) const{
    return !parts.empty() && parts.size() >= 2 && parts[0] == "uniforms";
  }

  //could this could be improved by returning a pointer
std::string buildUniformPath(const std ::vector<std::string>& parts) {
    std::string uniformName;
    for (size_t i = 1; i < parts.size(); ++i) {
      if (i > 1)
        uniformName += ".";
      if (i == parts.size() - 1 && parts[i] == "heading") {
        uniformName += "direction";
      } else {
        uniformName += parts[i];
      }
    }
    return uniformName;
  }

std::optional<ApiValue> UniformPathHandler::set(
    const std::vector<std::string>& parts,
    const ApiValue& value) {

  if (parts.size() < 2) {
    return std::nullopt; // not enough parts to be meaningful
  }

  const std::string& targetShader = parts[1];
  bool convertFromHeading = !parts.empty() && parts.back() == "heading";

  auto uniformName = buildUniformPath(parts);
  ApiValue val = value;
  if (convertFromHeading) {
    val = headingToVec3(std::get<float>(value));
  }

  std::optional<ApiValue> result;
  bool targeted = std::find(shaderNames.begin(), shaderNames.end(), targetShader) != shaderNames.end();

  if (targeted) {
    result = uniformState.setUniform(targetShader, parts.back(), val);
  } else {
    for (const auto& shader : shaderNames) {
      result = uniformState.setUniform(shader, uniformName, val);
    }
  }

  if (convertFromHeading && result && std::holds_alternative<std::vector<float>>(*result)) {
    const auto& vec = std::get<std::vector<float>>(*result);
    if (vec.size() >= 3) {
      return vecToHeading(vec[0], vec[1]);
    }
  }

  return result;
}

  std::optional<ApiValue> UniformPathHandler::get(
      const std::vector<std::string>& parts) {
    bool convertToHeading = parts.size() >= 3 && parts[2] == "heading";
   auto uniformName = buildUniformPath(parts);

    std::optional<ApiValue> result;
    if (std::find(shaderNames.begin(), shaderNames.end(), parts[1]) !=
        shaderNames.end()) {
      result = uniformState.getUniform(parts[1], parts[2]);
    } else {
      for (const auto& shader : shaderNames) {
        result = uniformState.getUniform(shader, uniformName);
        if (result)
          break;
      }
    }
    if (convertToHeading &&
        std::holds_alternative<std::vector<float>>(*result)) {
          const auto& vec = std::get<std::vector<float>>(*result);
          if(vec.size() >= 3){
            return vecToHeading(vec[0],vec[1]);
          }
    }
    return result;
  }
