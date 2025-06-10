#include "OceanApi.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include "OceanGestalt.hpp"
#include "Configuration.hpp"

OceanApi::OceanApi(std::shared_ptr<OceanGestalt> app) : app(app) {}

void OceanApi::pauseSimulation(bool pause) {
  std::cout << "pause " << pause << std::endl;
  // we can call into the app to do whatever we need to
  app->toggleSimulation();
}

void OceanApi::updateSimulation(std::string path, std::string value) {
  
  std::cout << "updating " << path << " with " << value << std::endl;
}

std::optional<std::string> OceanApi::setUniform(std::string shaderName,
                                                std::string uniformName,
                                                std::string value) {
  // the first part of the path will be the mesh to which this applies
  return std::string(value);
}

std::optional<std::string> OceanApi::setUniform(std::string shaderName,
                                                std::string uniformName,
                                                float value) {
  // the first part of the path will be the mesh to which this applies
  return std::string("0.23");
}

std::optional<std::string> OceanApi::setUniform(std::string shaderName,
                                                std::string uniformName,
                                                std::vector<float> const) {
  return std::string(".12");
}

std::optional<std::string> OceanApi::getUniform(std::string shaderName, std::string uniformName) {
  auto configuration = app->getConfiguration();
  shared_ptr<ShaderProgram> program; 
  if(shaderName == "mesh"){
    program = configuration.meshShader;
  }else if (shaderName == "wireframe"){
    program = configuration.wireframeShader;
  }
  return program->getUniform(uniformName);
}