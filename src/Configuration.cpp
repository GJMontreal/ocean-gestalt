#include "Configuration.hpp"
#include "Camera.hpp"
#include "jsonloader.hpp"  //rename
#include "asset.hpp"

#include <memory.h>
#include <fstream>
#include <iostream>

using json = nlohmann::json;
using namespace std;



Configuration::Configuration(const string& fileName){
  loadWaves(fileName);
  loadCamera(fileName);
  loadShaders(fileName);
  loadLight(fileName);
}

int Configuration::loadWaves(const string& fileName){
  json data;
  int retval = loadJSON(fileName, data);
  if(0==retval){
    for( const auto& element: data.at("waves"))
      {
        auto wave = std::make_shared<Wave>();
        element.get_to(*(wave.get()));
        waves.push_back(std::move(wave));
      }
  }
  return retval;
}

int Configuration::loadCamera(const string& fileName){
  json data;
  int retval = loadJSON(fileName, data);
  if(0==retval){
   auto aCamera = std::make_shared<Camera>();
      json j =  data.at("camera");
      j.get_to(*(aCamera.get()));
      camera = std::move(aCamera);
  }
  return retval;
}

int Configuration::loadJSON(const string& fileName, json& data) const{
  int retval = 0;
  ifstream file(fileName);
  if (file) {
    data = json::parse(file);
  } else {
    throw std::invalid_argument(string("The file ") + fileName +
                                " doesn't exist");
    retval = -1;
  }
  return retval;
}

void Configuration::loadShaders(const string& fileName){
  //TODO: pass the filenames for each shader in a json file
  Shader vertexShader(SHADER_DIR "/gerstner.vert", GL_VERTEX_SHADER);
  Shader fragmentShader(SHADER_DIR "/shader.frag", GL_FRAGMENT_SHADER);
  auto program = std::make_shared<ShaderProgram>(ShaderProgram({vertexShader,fragmentShader}));
  meshShader = std::move(program);

  vertexShader = Shader(SHADER_DIR "/gerstner.vert", GL_VERTEX_SHADER);
  fragmentShader = Shader(SHADER_DIR "/simple.frag", GL_FRAGMENT_SHADER);
  program = std::make_shared<ShaderProgram>(ShaderProgram({vertexShader,fragmentShader}));
  wireframeShader = std::move(program);

  Shader geometryShader(SHADER_DIR "/gerstner_normal.gs", GL_GEOMETRY_SHADER);
  vertexShader = Shader(SHADER_DIR "/gerstner.vert", GL_VERTEX_SHADER);
  fragmentShader = Shader(SHADER_DIR "/simple.frag", GL_FRAGMENT_SHADER);
  program = std::make_shared<ShaderProgram>(ShaderProgram({vertexShader,fragmentShader,geometryShader}));
  normalShader = std::move(program);
}

int Configuration::loadLight(const string& fileName){
  json data;
  int retval = loadJSON(fileName, data);
  if(0==retval){
      json j =  data.at("light");
      j.get_to(lightPosition);
  }
  return retval;
}
