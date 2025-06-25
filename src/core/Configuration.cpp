#include "Configuration.hpp"
#include "Camera.hpp"
#include "Serialization.hpp"
#include "asset.hpp"
#include "ApiAdapter.hpp"
#include "Shader.hpp"
#include "UniformValue.hpp"
#include "Utilities.hpp"

#include <memory>
#include <fstream>
#include <iostream>
#include <thread>
#include <future>

using nlohmann::json;
using std::cout;
using std::endl;
using std::make_shared;

Configuration::Configuration(const string& environment,
                             const string& shader,
                             const string& generator,
                             const string& api) {
  loadWaves(environment);
  loadCamera(environment);
  loadLight(environment);
  loadMesh(environment);
  loadShaders(shader);
  loadGenerator(generator);
  loadAPISettings(api);
}

void Configuration::setApi(std::shared_ptr<ApiAdapter> api) {
  this->api = api;
}

void Configuration::loadWaves(const string& fileName) {
  json data;
  loadJSON(fileName, data);

  for (const auto& element : data.at("waves")) {
    WaveSerialized serialized;
    element.get_to(serialized);
    auto wave = make_shared<Wave>(serialized);
    waves.push_back(std::move(wave));
  }
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

  meshShader = buildShader(j, "mesh_shader", meshColor);
  wireframeShader = buildShader(j, "wireframe_shader", wireframeColor);
  shaders[meshShader->getName()] = meshShader;
  shaders[wireframeShader->getName()] = wireframeShader;

  // WEBGL doesn't support geometry shaders
#ifndef __EMSCRIPTEN__
  normalShader = buildShader(j, "normal_shader", normalColor);
#endif
  shaders[normalShader->getName()] = normalShader;

  // we should get rid of this colour stuff?
  vec4 drawableColor;
  for (auto name : {"drawable_normal", "drawable_mesh"}) {
    auto shader = buildShader(j, name, drawableColor);
    shaders[shader->getName()] = shader;
  }
}

shared_ptr<ShaderProgram> Configuration::buildShader(json& j,
                                                     const string& name,
                                                     vec4& color) {
  auto shaderJSON = j.at(name);

  Shader vertexShader(SHADER_DIR + (string)shaderJSON.at("vertex"),
                      GL_VERTEX_SHADER);
  Shader fragmentShader(SHADER_DIR + (string)shaderJSON.at("fragment"),
                        GL_FRAGMENT_SHADER);

  color = shaderJSON.at("color"); // TODO: I think we should be able to get rid of this with our new uniform system
  // optional geometry shader where supported
  auto geometry = shaderJSON["geometry"];
  shared_ptr<ShaderProgram> program;
  if (geometry != nullptr) {
    Shader geometryShader(SHADER_DIR + (string)geometry, GL_GEOMETRY_SHADER);
    program = make_shared<ShaderProgram>(
        ShaderProgram(name,{vertexShader, fragmentShader, geometryShader}));
  } else {
    program =
        make_shared<ShaderProgram>(ShaderProgram(name,{vertexShader, fragmentShader}));
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
    json data = *this; // I've already forgotten how this works
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

void Configuration::setWaveParameter(const std::string& key, const ApiValue& value){
  std::cout << "set wave parameters" << std::endl;
  // I wonder what thread we're on here
  // we'll need a mutex to guard
}

template <typename F>
void dispatch_async(F&& task) {
  std::thread(std::forward<F>(task)).detach();  //fire and forget
}

void Configuration::setInitialUniformState(const ApiAdapter& api){
  //set all the uniforms
  // api.setValue("uniforms.mesh_shader.lineColor",  uniformToApi(meshColor));
    api.setValue("uniforms.wireframe_shader.lineColor", uniformToApi(wireframeColor));

    // these need to be set before the render loop runs
    for(auto value: {shaders["mesh_shader"],shaders["wireframe_shader"],shaders["normal_shader"]}){
      value->activate();
      value->loadTexture(SHADER_DIR "gust_noise_512.png","gustNoise");
      value->loadTexture(SHADER_DIR "NormalMap.png","gustNormalMap");
      value->deactivate();
    }
    
    dispatch_async([this, &api] {
      this->setInitialWaveUniforms(api);
    });
}

void Configuration::setInitialWaveUniforms(const ApiAdapter& api)const{
     api.setValue("uniforms.gust.direction",
                   uniformToApi(glm::vec3(-1.0, 0.3, 0.0)));
      api.setValue("uniforms.gust.strength", uniformToApi(0.0f));
      api.setValue("uniforms.gust.speed", uniformToApi(0.0f));
      api.setValue("uniforms.gust.scale", 0.052f);
      
      for(auto shaderName: {"mesh_shader","wireframe_shader","normal_shader"}){
     int i = 0;
        for (const shared_ptr<Wave> wave : waves) {
        std::string uniformName =
            string_format("uniforms.%s.waves[%i].amplitude", shaderName, i);
        api.setValue(uniformName, uniformToApi(wave->amplitude));

        uniformName = string_format("uniforms.%s.waves[%i].steepness", shaderName,i);
        api.setValue(uniformName, uniformToApi(wave->steepness));
        uniformName = string_format("uniforms.%s.waves[%i].wavelength", shaderName, i);
        api.setValue(uniformName, uniformToApi(wave->wavelength));
        uniformName = string_format("uniforms.%s.waves[%i].direction", shaderName, i);
        api.setValue(uniformName,
                     uniformToApi(glm::vec3(wave->direction, 0.0f)));

        i++;
      }
    }
}

std::shared_ptr<WaveInterface> Configuration::getWaveInterface() {
  return nullptr;
}

std::vector<std::shared_ptr<Wave>> Configuration::getWaves() {
  return waves;
}
