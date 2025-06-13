#include "PathHandler.hpp"

  bool UniformPathHandler::matches(const std::vector<std::string>& parts) const{
    return !parts.empty() && parts.size() > 1 && parts[0] == "uniforms";
  }

  std::optional<ApiValue> UniformPathHandler::set(
      const std::vector<std::string>& parts,
      const ApiValue& value) {
        if(parts.size() == 2){
          //applies to all shaders
          return std::nullopt;
        }else if (parts.size() == 3){
         uniformState.setUniform(parts[1], parts[2], value);
        }
    return std::nullopt;
  }

  std::optional<ApiValue> UniformPathHandler::get(
      const std::vector<std::string>& parts) {
    return std::nullopt;
  }
