#include "OceanApi.hpp"


#include <iostream>
#include <string>

#include "ConfigurationInterface.hpp"
#include "OceanGestaltInterface.hpp"
    
OceanApi::OceanApi(std::shared_ptr<OceanGestaltInterface> app) : app(app) {
  // TODO: this might be a good time to build a map of our uniforms
  // though I suppose it depends on when things are instantiated
  // I could see all of this here happening well before the shaders are compiled
}

void OceanApi::setupShaderNormalInterface(){
  std::cout << "Setting up shader normals" << std::endl;
}

void OceanApi::pauseSimulation(bool pause) {
  std::cout << "pause " << pause << std::endl;
  // we can call into the app to do whatever we need to
  // app->toggleSimulation();
}

void OceanApi::updateSimulation(std::string path, std::string value) {
  
  std::cout << "updating " << path << " with " << value << std::endl;
}

std::optional<std::string> OceanApi::setUniform(
  const std::string_view& shaderName,
  const std::string_view& uniformName,
  const std::string_view& value) {
  // the first part of the path will be the mesh to which this applies
  return std::string(value);
}

std::optional<float> OceanApi::setUniform(
  const std::string_view& shaderName,
  const std::string_view& uniformName,
  float value) {
  // the first part of the path will be the mesh to which this applies
  return value;
}

std::optional<std::vector<float> > OceanApi::setUniform(
  const std::string_view& shaderName,
  const std::string_view& uniformName,
  std::vector<float> const value) {
  return value;
}

std::optional<bool> OceanApi::setUniform (
  const std::string_view& shaderName,
  const std::string_view& uniformName,
  bool value
){
  return value;
}

std::optional<std::string> OceanApi::getUniform(std::string shaderName, std::string uniformName) {
  auto configuration = app->getConfiguration();
  std::shared_ptr<ShaderProgram> program; 
  if(shaderName == "mesh"){
    program = configuration.meshShader;
  }else if (shaderName == "wireframe"){
    program = configuration.wireframeShader;
  }
  return program->getUniform(uniformName);
}