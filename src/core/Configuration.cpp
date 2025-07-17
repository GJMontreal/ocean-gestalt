#include "Configuration.hpp"
#include "Camera.hpp"
#include "Serialization.hpp"
#include "asset.hpp"
#include "ApiAdapter.hpp"
#include "Shader.hpp"
#include "UniformValue.hpp"
#include "Utilities.hpp"
#include "TextRenderer.hpp"
#include <memory>
#include <fstream>
#include <iostream>
#include <thread>
#include <future>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using nlohmann::json;
using std::cout;
using std::endl;
using std::make_shared;
namespace {
constexpr int NUM_WAVES = 10;
const std::string VERTEX_KEY = "vertex";
const std::string FRAGMENT_KEY = "fragment";
const std::string GEOMETRY_KEY = "geometry";
const std::string TEXTURE_KEY = "texture";
const std::string CUBEMAP_KEY = "cubemap";
const std::string COLOR_KEY = "color";
}  // namespace

Configuration::Configuration(const string& environment,
                             const string& shader,
                             const string& generator,
                             const string& api) {
  loadCamera(environment);
  loadLight(environment);
  loadMesh(environment);
  loadShaders(shader);
  loadAPISettings(api);

  waves.reserve(NUM_WAVES);
  for(int i = 0; i < NUM_WAVES; i++){
    waves.push_back(make_shared<Wave>());
  }

  textRenderer = make_shared<TextRenderer>(FONT_DIR "FiraCode-Regular.ttf",64.f);
}

void Configuration::setApi(std::shared_ptr<ApiAdapter> api) {
  this->api = api;
}

std::shared_ptr<ApiAdapter> Configuration::getApi() {
  auto locked = api.lock();
  return locked;
}

void Configuration::loadCamera(const string& fileName) {
  json data;
  loadJSON(fileName, data);

  auto aCamera = make_shared<Camera>();
  auto j = data.at("camera");
  j.get_to(*(aCamera.get()));
  camera = std::move(aCamera);
}

void Configuration::loadJSON(const string& fileName, json& data) const {
  std::ifstream file(fileName);
  if (file) {
    data = json::parse(file);
  } else {
    // If we want to catch this in the browser, pass fexceptions at compile and
    // link time
    throw std::invalid_argument(string("The file ") + fileName +
                                " doesn't exist");
  }
}

void Configuration::loadShaders(const string& fileName) {
  json j;
  loadJSON(fileName, j);

  // the top level keys should be shaderNames
  for(auto& [shaderName,value] : j.items()){
  #ifdef __EMSCRIPTEN__
    if(value.contains("geometry")){
      continue;
    }
  #endif
    //build each shader
    vec4 color;  //TODO: Change the way this works
    auto shader = buildShader(j, shaderName, color);
    shaders[shaderName] = shader;
  }
  //TODO: Find a better way to handle the colors
  meshColor = glm::vec4(0.10, 0.2, 0.25, 1);
}

shared_ptr<ShaderProgram> Configuration::buildShader(json& j,
                                                     const string& name,
                                                     vec4& color) {
  auto shaderJSON = j.at(name);

  Shader vertexShader(SHADER_DIR + (string)shaderJSON.at("vertex"),
                      GL_VERTEX_SHADER);
  Shader fragmentShader(SHADER_DIR + (string)shaderJSON.at("fragment"),
                        GL_FRAGMENT_SHADER);

  // are colors associated with shaders, or models - models I think
  //TODO: decide what to do here with the color
  // color should be optional
  if (shaderJSON.contains(COLOR_KEY)) {
    color = shaderJSON.at(COLOR_KEY);
  }

  // optional geometry shader where supported
  auto geometry = shaderJSON[GEOMETRY_KEY];
  shared_ptr<ShaderProgram> program;
  if (geometry != nullptr) {
    Shader geometryShader(SHADER_DIR + (string)geometry, GL_GEOMETRY_SHADER);
    program = make_shared<ShaderProgram>(
        ShaderProgram(name,{vertexShader, fragmentShader, geometryShader}));
  } else {
    program =
        make_shared<ShaderProgram>(ShaderProgram(name,{vertexShader, fragmentShader}));
  }

  GLuint unit = 0;
  // any textures 
  if (shaderJSON.contains(TEXTURE_KEY)){
    auto textures = shaderJSON.at(TEXTURE_KEY);
    for(auto &[uniform, texture] : textures.items()){
      std::string path = TEXTURE_DIR + texture.get<std::string>();
      program->loadTexture(path, uniform,unit);
      unit++;  //TODO: we should track textures and units for reuse and sharing
    }
  }

  if (shaderJSON.contains(CUBEMAP_KEY)){
    auto textures = shaderJSON.at(CUBEMAP_KEY);
    for(auto &[uniform, cubemap] : textures.items()){
      std::string path = TEXTURE_DIR + cubemap.get<std::string>();
      program->loadCubemap(path, uniform, unit);
      unit++;
    }
  }

  return program;
}

void Configuration::loadLight(const string& fileName) {
  json data;
  loadJSON(fileName, data);
  auto j = data.at("light");
  j.get_to(lightPosition);
}

void Configuration::loadMesh(const string& fileName) {
  json data;
  loadJSON(fileName, data);
  auto mesh = data.at("mesh");
  meshSize = mesh.at("size");
  meshSubdivisions = mesh.at("subdivisions");
}

void Configuration::loadGenerator(const string& fileName) {
  json data;
  loadJSON(fileName, data);

  medianWavelength = data.at("median_wavelength");
  medianAmplitude = data.at("median_amplitude");
  directionalVariance = data.at("directional_variance");
  stdDeviation = data.at("std_deviation");
}

void Configuration::loadAPISettings(const string& fileName) {
  json data;
  loadJSON(fileName, data);
  port = data.at("port");
}

void Configuration::dumpUniforms(const string& fileName) {
  if (auto locked = api.lock()) {
    std::ofstream file(fileName);
    if (file) {
      auto&& uniforms = locked->dumpUniforms();
      json data = *uniforms;
      cout << "Writing uniforms" << endl;
      file << std::setw(4) << data << endl;
      file.close();
    } else {
      throw std::invalid_argument(string("The file ") + fileName +
                                  " doesn't exist");
    }
  }
}

void Configuration::loadUniforms(const string& fileName) {
  if (auto locked = api.lock()) {
    json j;
    loadJSON(fileName, j);
    for (auto it = j.begin(); it != j.end(); ++it) {
      const std::string& key = "uniforms." + it.key() ;
      const json& value = it.value();
      ApiValue apiVal = value.get<ApiValue>();
      locked->setValue(key, apiVal);
    }
  }
}

void Configuration::save(const string& fileName) {
  std::ofstream file(fileName);
  if (file) {
    cout << "Writing configuration " << endl;
    json data = *this;
    file << std::setw(4) << data << endl;
    cout << std::setw(4) << data
         << endl;  // so we can dump this in the web console
    file.close();
  } else {
    throw std::invalid_argument(string("The file ") + fileName +
                                " doesn't exist");
  }
}

std::unordered_map<std::string, shared_ptr<ShaderProgram>>& Configuration::getShaders(){
  return shaders;
};

std::shared_ptr<ShaderProgram> Configuration::getShader(const std::string& shaderName) {
  //TODO: what do we do in the case the shader doesn't exist?
  return getShaders()[shaderName];
}

void Configuration::setWaveParameter(const std::string& key, const ApiValue& value){
  std::cout << "set wave parameters" << std::endl;
}

template <typename F>
void dispatch_async(F&& task) {
  std::thread(std::forward<F>(task)).detach();  //fire and forget
}


// TODO: this looks like a duplication of code in app with the exception of the file names
void Configuration::setInitialUniformState(const ApiAdapter& api){
#ifndef __EMSCRIPTEN__
  dispatch_async([this, &api] {
      std::cout << "Loading uniforms" << std::endl;
      this->loadUniforms(CONFIGURATION_DIR "uniforms_min.json");
    });
#else
    auto* that = this;
  emscripten_async_call([](void* arg) {
    auto* self = static_cast<Configuration*>(arg);
    std::cout << "Loading uniforms" << std::endl;
    self->loadUniforms(CONFIGURATION_DIR "uniforms.json");
  }, that, 0);  // delay = 0 ms
#endif
}

void Configuration::setInitialWaveUniforms(const ApiAdapter& api)const{
   
}

std::shared_ptr<WaveInterface> Configuration::getWaveInterface() {
  return nullptr;
}

std::vector<std::shared_ptr<Wave>> Configuration::getWaves() {
  return waves;
}
