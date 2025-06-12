#include "OceanApi.hpp"


#include <iostream>
#include <string>

#include "OceanGestaltInterface.hpp"
#include "ConfigurationInterface.hpp"

#include "Shader.hpp"
OceanApi::OceanApi(OceanGestaltInterface& app) : app(app) {
  // TODO: this might be a good time to build a map of our uniforms
  // though I suppose it depends on when things are instantiated
  // I could see all of this here happening well before the shaders are compiled
  // do we have access to the shader programs

  
  auto wireframe = std::make_unique<UniformInspector>(*(app.getConfiguration().wireframeShader));
  auto mesh = std::make_unique<UniformInspector>(*(app.getConfiguration().meshShader));
  shaderUniforms["mesh"] = std::move(mesh);
  shaderUniforms["wireframe"] = std::move(wireframe); 
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

std::optional<UniformValue> OceanApi::setUniform(
  const std::string_view& shaderName,
  const std::string_view& uniformName,
  UniformValue value) {
  // the first part of the path will be the mesh to which this applies
  return value;
}

std::optional<std::any> OceanApi::getUniform(std::string shaderName, std::string uniformName) {
  auto configuration = app.getConfiguration();
  std::shared_ptr<ShaderProgram> program; 
  if(shaderName == "mesh"){
    program = configuration.meshShader;
  }else if (shaderName == "wireframe"){
    program = configuration.wireframeShader;
  }
  return program->getUniform(uniformName);
}
