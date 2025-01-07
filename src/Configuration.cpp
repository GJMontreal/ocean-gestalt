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

static const auto SHADERS = vector<string>{"mesh_shader","wireframe_shader","normal_shader"};

Configuration::Configuration(const string& environment, const string& shader, const string &generator){
  loadWaves(environment);
  loadCamera(environment);
  loadLight(environment);
  loadMeshSize(environment);
  loadShaders(shader);
  loadGenerator(generator);
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
  json j;
  loadJSON(fileName, j);

  meshShader = buildShader(j, "mesh_shader", meshColor );
  wireframeShader = buildShader(j,"wireframe_shader", wireframeColor);

   // WEBGL doesn't support geometry shaders
  #ifndef __EMSCRIPTEN__
  normalShader = buildShader(j,"normal_shader",normalColor);
  #endif
}

shared_ptr<ShaderProgram> Configuration::buildShader(json& j, const string& name, vec4& color){
  auto shaderJSON = j.at(name);
 
  Shader vertexShader(SHADER_DIR + (string)shaderJSON.at("vertex"), GL_VERTEX_SHADER);
  Shader fragmentShader(SHADER_DIR + (string)shaderJSON.at("fragment"), GL_FRAGMENT_SHADER);

  color = shaderJSON.at("color");

  //optional geometry shader where supported
  auto geometry = shaderJSON["geometry"];
  shared_ptr<ShaderProgram> program;
  if(geometry != nullptr){
    Shader geometryShader(SHADER_DIR + (string)geometry, GL_GEOMETRY_SHADER);
    program = make_shared<ShaderProgram>(ShaderProgram{vertexShader,fragmentShader, geometryShader});
  }else{
    program = make_shared<ShaderProgram>(ShaderProgram{vertexShader, fragmentShader});
  }
  return program;
}

void Configuration::loadLight(const string& fileName){
  json data;
  loadJSON(fileName, data);
  vec3 lightPosition; 
  auto j =  data.at("light");
  j.get_to(lightPosition);
  light = make_shared<Light>(lightPosition);
}

void Configuration::loadMeshSize(const string& fileName){
  json data;
  loadJSON(fileName, data);

  auto j = data.at("mesh_size");
  j.get_to(meshSize);
}

void Configuration::loadGenerator(const string& fileName){
  json data;
  loadJSON(fileName, data);

  medianWavelength = data.at("median_wavelength");
  medianAmplitude = data.at("median_amplitude");
  directionalVariance = data.at("directional_variance");
  stdDeviation = data.at("std_deviation");
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