#include "OceanApi.hpp"

#include "AppContextInterface.hpp"
#include "OceanGestaltInterface.hpp"
#include "GetKeys.hpp"

#include <iostream>
#include <string>
#include <sstream>

OceanApi::OceanApi(OceanGestaltInterface& app, UniformState& state)
    : app(app), uniformState(state){
  
  auto& context = app.getContext();
  auto& shaders = context.getShaders();
  auto keys = getKeys(shaders);
  auto uniformPathHandler = std::make_unique<UniformPathHandler>(state,keys);
  pathHandlers.push_back(std::move(uniformPathHandler));
};

void OceanApi::setupShaderNormalInterface() {
  std::cout << "Setting up shader normals" << std::endl;
}

void OceanApi::pauseSimulation(bool pause) {

  app.pauseSimulation(pause);
}

void OceanApi::updateSimulation(std::string path, std::string value) {
  
  std::cout << "updating " << path << " with " << value << std::endl;
}

std::optional<ApiValue> OceanApi::setValue(const std::string& path, ApiValue& value){
  auto parts = splitPath(path);
  for(auto& handler:pathHandlers){
    if(handler->matches(parts)){
      return handler->set(parts, value);
    }
  }
  return std::nullopt;
};

std::optional<ApiValue> OceanApi::setUniform(
  const std::string& shaderName,
  const std::string& uniformName,
  ApiValue value) {
  
  return uniformState.setUniform(shaderName, uniformName, value);
}

std::optional<std::any> OceanApi::getUniform(std::string shaderName, std::string uniformName) {
  // auto configuration = app.getContext();
  // std::shared_ptr<ShaderProgram> program; 
  // if(shaderName == "mesh"){
  //   program = configuration.meshShader;
  // }else if (shaderName == "wireframe"){
  //   program = configuration.wireframeShader;
  // }
  // return program->getUniform(uniformName);
  return std::nullopt;
}

std::vector<std::string> OceanApi::splitPath(const std::string& input, char delimiter) {
  std::vector<std::string> parts;
  std::stringstream ss(input);
  std::string part;

  while (std::getline(ss, part, delimiter)) {
    if (!part.empty()) {
      parts.push_back(part);
    }
  }

  return parts;
}
