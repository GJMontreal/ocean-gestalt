#include "PathHandler.hpp"
#include "AppContextInterface.hpp"

  bool UniformPathHandler::matches(const std::vector<std::string>& parts) const{
    return !parts.empty() && parts.size() > 2 && parts[0] == "uniforms";
  }

  // we need to check if the first element in the name of a shader
  // if not it will apply to all shaders
  std::optional<ApiValue> UniformPathHandler::set(
      const std::vector<std::string>& parts,
      const ApiValue& value) {
    if (std::find(shaderNames.begin(), shaderNames.end(), parts[1]) != shaderNames.end()){
        return uniformState.setUniform(parts[1], parts[2], value);
      } else{
        std::optional<ApiValue> result;
        auto uniformName = parts[1] +"." + parts[2]; //this should properly append any parts
        for(const auto& shader: shaderNames){
          result = uniformState.setUniform(shader, uniformName, value);
        }
        return result;
      }
  }

  std::optional<ApiValue> UniformPathHandler::get(
      const std::vector<std::string>& parts) {
    if (std::find(shaderNames.begin(), shaderNames.end(), parts[1]) != shaderNames.end()){
        return uniformState.getUniform(parts[1], parts[2]);
      } else{
        std::optional<ApiValue> result;
        auto uniformName = parts[1] +"." + parts[2]; //this should properly append any parts
        for(const auto& shader: shaderNames){
          result = uniformState.getUniform(shader, uniformName);
        }
        return result;
      }
  }
