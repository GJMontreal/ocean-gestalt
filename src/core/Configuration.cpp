#include "Configuration.hpp"
#include "Camera.hpp"
#include "Serialization.hpp"
#include "asset.hpp"
#include "ApiAdapter.hpp"
#include "Shader.hpp"
#include "UniformValue.hpp"
#include "Utilities.hpp"
#include "TextRenderer.hpp"
#include "UniformAnimator.hpp"
#include "PatternMatch.hpp"
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
  loadScene(environment);
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

void Configuration::setAnimator(std::shared_ptr<UniformAnimator> a) {
  animator = a;
}

std::shared_ptr<UniformAnimator> Configuration::getAnimator() {
  return animator.lock();
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
  if (data.contains("reflection")) {
    reflectionSize = data.at("reflection").at("size");
  }
  if (data.contains("shadow")) {
    shadowSize = data.at("shadow").at("size");
  }
}

void Configuration::loadScene(const string& fileName) {
  json data;
  loadJSON(fileName, data);
  if (!data.contains("models")) return;
  for (auto& m : data.at("models")) {
    SceneModelConfig cfg;
    cfg.name   = m.at("name");
    cfg.file   = m.at("file");
    cfg.shader = m.value("shader", std::string{});
    if (m.contains("position")) cfg.position = m.at("position").get<glm::vec3>();
    if (m.contains("rotation")) cfg.rotation = m.at("rotation").get<glm::vec3>();
    if (m.contains("scale"))    cfg.scale    = glm::vec3(m.at("scale").get<float>());
    cfg.floating       = m.value("floating", true);
    cfg.tethered       = m.value("tethered", false);
    cfg.rightingWeight     = m.value("rightingWeight",     0.5f);
    cfg.spinTorqueScale    = m.value("spinTorqueScale",    2.0f);
    cfg.angularDamping     = m.value("angularDamping",     1.5f);
    cfg.torsionalStiffness = m.value("torsionalStiffness", 0.5f);
    sceneModels.push_back(cfg);
  }
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
    auto anim = animator.lock();

    std::optional<float> globalDuration;
    if (j.contains("duration") && j["duration"].is_number()) {
      globalDuration = j["duration"].get<float>();
    }

    std::vector<std::string> animatePatterns;
    if (j.contains("animate") && j["animate"].is_array()) {
      animatePatterns = j["animate"].get<std::vector<std::string>>();
    }

    for (auto it = j.begin(); it != j.end(); ++it) {
      if (it.key() == "duration" || it.key() == "animate") continue;

      const std::string key = "uniforms." + it.key();
      const json& value = it.value();

      std::optional<float> duration;
      ApiValue target;

      if (value.is_object() && value.contains("value") && value.contains("duration")) {
        target   = value["value"].get<ApiValue>();
        duration = value["duration"].get<float>();
      } else {
        target = value.get<ApiValue>();
        if (!animatePatterns.empty() && matchesAnyPattern(animatePatterns, it.key())) {
          duration = globalDuration;
        }
      }

      bool isAnimatable = std::holds_alternative<float>(target) ||
                          std::holds_alternative<std::vector<float>>(target);

      if (anim && duration && isAnimatable) {
        // Default from-value must match the type of target so lerpValue doesn't throw.
        ApiValue zero = std::visit([](auto&& v) -> ApiValue {
          using T = std::decay_t<decltype(v)>;
          if constexpr (std::is_same_v<T, std::vector<float>>)
            return std::vector<float>(v.size(), 0.0f);
          else
            return T{};
        }, target);
        ApiValue from = locked->getValue(key).value_or(zero);
        std::shared_ptr<ApiAdapter> apiPtr = locked;
        anim->animateTo(from, target, *duration,
                        [apiPtr, key](const ApiValue& v) {
                          apiPtr->setValue(key, v, false);
                        }, key);
      } else {
        locked->setValue(key, target);
      }
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
    self->loadUniforms(CONFIGURATION_DIR "uniforms_web.json");
  }, that, 0);  // delay = 0 ms
#endif
}

std::shared_ptr<WaveInterface> Configuration::getWaveInterface() {
  return nullptr;
}

std::vector<std::shared_ptr<Wave>>& Configuration::getWaves() {
  return waves;
}
