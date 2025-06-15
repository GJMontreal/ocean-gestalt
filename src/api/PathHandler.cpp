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
    return !parts.empty() && parts.size() > 2 && parts[0] == "uniforms";
  }


  // we need to check if the first element in the name of a shader
  // if not it will apply to all shaders
  std::optional<ApiValue> UniformPathHandler::set(
      const std::vector<std::string>& parts,
      const ApiValue& value) {
        ApiValue val = value;
        //convert directions from float to vec3
        if (parts.size() >= 3 && parts[2] == "heading") {
          val = headingToVec3(std::get<float>(value));
        }

    if (std::find(shaderNames.begin(), shaderNames.end(), parts[1]) != shaderNames.end()){
      return uniformState.setUniform(parts[1], parts[2], val);
    } else {
      std::optional<ApiValue> result;
      auto uniformName =
          parts[1] + "." + parts[2];  // this should properly append any parts
      for (const auto& shader : shaderNames) {
        result = uniformState.setUniform(shader, uniformName, val);
      }
      return result;
    }
  }

  std::optional<ApiValue> UniformPathHandler::get(
      const std::vector<std::string>& parts) {
    bool convertToHeading = parts.size() >= 3 && parts[2] == "heading";
    std::string uniformName =
        parts[1] + "." + (convertToHeading ? "direction" : parts[2]);

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
