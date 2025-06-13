#include "OceanApi.hpp"

#include "AppContextInterface.hpp"
#include "OceanGestaltInterface.hpp"
#include "Shader.hpp"

#include <iostream>
#include <string>


void OceanApi::setupShaderNormalInterface(){
  std::cout << "Setting up shader normals" << std::endl;
}

void OceanApi::pauseSimulation(bool pause) {
  app.pauseSimulation(pause);
}

void OceanApi::updateSimulation(std::string path, std::string value) {
  
  std::cout << "updating " << path << " with " << value << std::endl;
}

std::optional<UniformValue> OceanApi::setUniform(
  const std::string& shaderName,
  const std::string& uniformName,
  UniformValue value) {
  
  return uniformState.setUniform(shaderName, uniformName, value);
}

std::optional<std::any> OceanApi::getUniform(std::string shaderName, std::string uniformName) {
  auto configuration = app.getContext();
  std::shared_ptr<ShaderProgram> program; 
  if(shaderName == "mesh"){
    program = configuration.meshShader;
  }else if (shaderName == "wireframe"){
    program = configuration.wireframeShader;
  }
  return program->getUniform(uniformName);
}
