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


  // make this a more generic parsePath which can be overriden
ParsedUniformPath UniformPathHandler::parseUniformPath(
      const std::vector<std::string>& parts) {
    ParsedUniformPath result;

    if (parts.size() < 2 || parts[0] != "uniforms") {
      return result;
    }

    auto partIt = parts.begin() + 1;
    auto partEnd = parts.end();

    // check if next part is a shader name
    auto maybeShaderName = *partIt;
    if (std::find(shaderNames.begin(), shaderNames.end(), maybeShaderName) !=
        shaderNames.end()) {
      result.shaderName = maybeShaderName;
      ++partIt;  // skip shader name
    }
    bool first = true;
    bool simRelated = false;
    for (; partIt != partEnd; ++partIt) {
      const bool isLast = (std::next(partIt) == partEnd);
      
      // this seems very fragile
      if (partIt->rfind("waves[",0)==0){
        simRelated = true;
      }
      
      if(simRelated && result.simulationPath){
        result.simulationPath = *result.simulationPath + ".";
      }

      if (!first) {
        result.internalPath += ".";
        result.externalPath += ".";
      } else {
        first = false;
      }

      if (isLast && *partIt == "heading") {
        result.internalPath += "direction";
        result.externalPath += "heading";
        result.convertHeading = true;
        if(simRelated){
          *result.simulationPath += "direction";
        }
      } else {
        result.internalPath += *partIt;
        result.externalPath += *partIt;
        if(simRelated){
          result.simulationPath = *result.simulationPath + *partIt;
        }
      }
    }

    return result;
  }

std::optional<ApiValue> UniformPathHandler::set(
    const std::vector<std::string>& parts,
    const ApiValue& value) {
  auto locked = uniformState.lock();
  if (!locked || parts.size() < 2) {
    return std::nullopt;
  }
  auto u = parseUniformPath(parts);

  ApiValue val = value;
  if (u.convertHeading) {
    val = headingToVec3(std::get<float>(value));
  }

  std::optional<ApiValue> result;

  if (u.shaderName) {
    result = locked->setUniform(*u.shaderName, u.internalPath, val);
  } else {
    for (const auto& shader : shaderNames) {
      result = locked->setUniform(shader, u.internalPath, val);
    }
  }

  // pass it on to our shared wave config
  if(u.simulationPath && syncWaveFunction) {
    syncWaveFunction(*u.simulationPath, val);
  }

  if (u.convertHeading && result &&
      std::holds_alternative<std::vector<float>>(*result)) {
    const auto& vec = std::get<std::vector<float>>(*result);
    if (vec.size() >= 3) {
      return vecToHeading(vec[0], vec[1]);
    }
  }

  return result;
}

std::optional<ApiValue> UniformPathHandler::get(
    const std::vector<std::string>& parts) {
  auto locked = uniformState.lock();
  if (!locked || parts.size() < 2) {
    return std::nullopt;
  }
  auto u = parseUniformPath(parts);
 
  std::optional<ApiValue> result;
  if (u.shaderName) {
    result = locked->getUniform(*u.shaderName,u.internalPath);
  } else { //if it's shared, just grab the first one that exists
    for (const auto& shader : shaderNames) {
      result = locked->getUniform(shader, u.internalPath);
      if (result)
        break;
    }
  }
  if (u.convertHeading && std::holds_alternative<std::vector<float>>(*result)) {
    const auto& vec = std::get<std::vector<float>>(*result);
    if (vec.size() >= 3) {
      return vecToHeading(vec[0], vec[1]);
    }
  }
  return result;
}
