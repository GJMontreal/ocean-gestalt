#include "Configuration.hpp"
#include "Camera.hpp"
#include "Serialization.hpp"
#include "asset.hpp"

#include <memory.h>
#include <fstream>
#include <iostream>

using nlohmann::json;
using std::cout;
using std::endl;
using std::make_shared;


Configuration::Configuration(const string& fileName){
  loadWaves(fileName);
  loadCamera(fileName);
  loadShaders(fileName);
  loadLight(fileName);
  loadShaderColors(fileName);
  loadMeshSize(fileName);
}

void Configuration::loadWaves(const string& fileName){
  json data;
  loadJSON(fileName, data);

  for( const auto& element: data.at("waves"))
    {
      WaveSerialized serialized;
      element.get_to(serialized);
      auto wave = make_shared<Wave>(serialized);
      waves.push_back(std::move(wave));
    }
}

void Configuration::loadCamera(const string& fileName){
  json data;
  loadJSON(fileName, data);
  
  auto aCamera = make_shared<Camera>();
  auto j =  data.at("camera");
  j.get_to(*(aCamera.get()));
  camera = std::move(aCamera);
}


void Configuration::loadJSON(const string& fileName, json& data) const{
  std::ifstream file(fileName);
  if (file) {
    data = json::parse(file);
  } else {
    //If we want to catch this in the browser, pass fexceptions at compile and link time
    throw std::invalid_argument(string("The file ") + fileName +
                                " doesn't exist");
  }
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

// WEBGL doesn't support geometry shaders
#ifndef __EMSCRIPTEN__
  Shader geometryShader(SHADER_DIR "/gerstner_normal.gs", GL_GEOMETRY_SHADER);
  vertexShader = Shader(SHADER_DIR "/gerstner.vert", GL_VERTEX_SHADER);
  fragmentShader = Shader(SHADER_DIR "/simple.frag", GL_FRAGMENT_SHADER);
  program = std::make_shared<ShaderProgram>(ShaderProgram({vertexShader,fragmentShader,geometryShader}));
  normalShader = std::move(program);
#endif
}

void Configuration::loadLight(const string& fileName){
  json data;
  loadJSON(fileName, data);
  vec3 lightPosition; 
  auto j =  data.at("light");
  j.get_to(lightPosition);
  light = make_shared<Light>(lightPosition);
}

void Configuration::loadShaderColors(const string& fileName){
  json data;
  loadJSON(fileName, data);
  
  auto j = data.at("normal_color");
  j.get_to(normalColor);

  j = data.at("mesh_color");
  j.get_to(meshColor);

  j = data.at("wireframe_color");
  j.get_to(wireframeColor);
}

void Configuration::loadMeshSize(const string& fileName){
  json data;
  loadJSON(fileName, data);

  auto j = data.at("mesh_size");
  j.get_to(meshSize);
}

void Configuration::save(const string& fileName){
  std::ofstream file(fileName);
  if(file){
    cout << "Writing configuration " << endl;
    json data = *this;
    file << std::setw(4) << data << endl;
    file.close();
  } else {
    throw std::invalid_argument(string("The file ") + fileName +
                                " doesn't exist");
  }
}