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

void OceanApi::setUniform(std::string path, std::string value) {
  // the first part of the path will be the mesh to which this applies
}

std::optional<std::string> OceanApi::getUniform(std::string shaderName, std::string path) {
  auto configuration = app->getConfiguration();
  shared_ptr<ShaderProgram> program; 
  if(shaderName == "mesh"){
    program = configuration.meshShader;
  }else if (shaderName == "wireframe"){
    program = configuration.wireframeShader;
  }
  return program->getUniform(path);
}