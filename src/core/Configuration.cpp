#include "Configuration.hpp"
#include "Camera.hpp"
#include "Serialization.hpp"
#include "asset.hpp"
#include "ApiAdapter.hpp"
#include "Shader.hpp"
#include "TextRenderer.hpp"

#include <memory>
#include <fstream>
#include <iostream>
#include <thread>
#include <future>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/fetch.h>
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

  auto j = data.at("camera");
  auto position = j.at("position").get<glm::vec3>();
  auto up = j.at("up").get<glm::vec3>();
  auto yaw = j.at("yaw").get<float>();
  auto pitch = j.at("pitch").get<float>();
  auto zoom = j.at("zoom").get<float>();

  camera = make_shared<Camera>(position, up, yaw, pitch, zoom);
  textRenderer = make_shared<TextRenderer>(FONT_DIR "FiraCode-Regular.ttf",64.f);
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

void Configuration::loadUniformsFromFile(const string& fileName) {

    json j;
    loadJSON(fileName, j);
    loadUniformsFromJSON(j);
}

void Configuration::loadUniformsFromJSON (const json& j){
  if (auto locked = api.lock()) {
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

void Configuration::setInitialUniformState(const ApiAdapter& api, const std::string& url){
#ifndef __EMSCRIPTEN__
  dispatch_async([this, &api] {
      std::cout << "Loading uniforms" << std::endl;
      this->loadUniformsFromFile(CONFIGURATION_DIR "uniforms_min.json");
    });
#else

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.userData = this;
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = [](emscripten_fetch_t* fetch) {

      std::string data(fetch->data, fetch->numBytes);
      std::cout << "fetched " << fetch->numBytes << std::endl;
      auto j = json::parse(data);
      auto configuration = static_cast<Configuration*>(fetch->userData);
      configuration->loadUniformsFromJSON(j);
      emscripten_fetch_close(fetch);
    };

    attr.onerror   = [](emscripten_fetch_t* fetch) {
      std::cerr << "Failed to fetch: " << fetch->url << std::endl;
      auto configuration = static_cast<Configuration*>(fetch->userData);
      emscripten_fetch_close(fetch);

      // load our defaults
      configuration->loadUniformsFromFile(CONFIGURATION_DIR "uniforms_web.json");
    };
    if( !url.empty()){
      emscripten_fetch(&attr, url.c_str());
    }else{
      loadUniformsFromFile(CONFIGURATION_DIR "uniforms_web.json");
    }
#endif
}

std::shared_ptr<WaveInterface> Configuration::getWaveInterface() {
  return nullptr;
}

std::vector<std::shared_ptr<Wave>>& Configuration::getWaves() {
  return waves;
}
