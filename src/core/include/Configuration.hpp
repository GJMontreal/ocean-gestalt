#ifndef __CONFIGURATION_HPP
#define __CONFIGURATION_HPP

#include "AppContextInterface.hpp"
#include "ApiValue.hpp"

#include "glm/glm.hpp"
#include <GL/glew.h>
#include <memory.h>
#include <nlohmann/json.hpp>
#include <string>

using nlohmann::json;
using glm::vec3;
using glm::vec4;
using std::shared_ptr;
using std::vector;
using std::string;

class Light;
class Wave;
class Camera;
class Shader;
class ShaderProgram;
class TextRenderer;

// This is our application context
// With the exception of the application, it should be held only weakly by other classes

struct SceneModelConfig {
  std::string name;
  std::string file;
  std::string shader;
  glm::vec3   position      = glm::vec3(0.0f);
  glm::vec3   rotation      = glm::vec3(0.0f); // Euler degrees (X, Y, Z)
  glm::vec3   scale         = glm::vec3(1.0f);
  bool        floating      = true;
  bool        tethered      = false;
  float       rightingWeight = 0.5f;
};

class Configuration: public AppContextInterface, public std::enable_shared_from_this<Configuration> {
 public:
  explicit Configuration(const string& environment,
    const string& shader,
    const string &generator,
    const string& apiConfig);  //why is api a string here?

  void setInitialUniformState(const ApiAdapter& api) override;
  vector<shared_ptr<Wave>> waves;

  shared_ptr<Camera> camera;
  
  //until we figure out how to handle this circularity
  vec3 lightPosition;
  std::shared_ptr<Light> light;
  
  std::shared_ptr<TextRenderer> textRenderer;

  std::vector<SceneModelConfig> sceneModels;

  int meshSize;
  int meshSubdivisions;
  int reflectionSize = 512;
  int shadowSize = 2048;
  
  float medianWavelength;
  float medianAmplitude;
  float directionalVariance;
  float stdDeviation;
  
  //TODO: these are normals - so they will move to UniformState
  vec4 meshColor;
  vec4 wireframeColor;
  vec4 normalColor;

  int port;

  void save(const string& fileName);
  void dumpUniforms(const string& fileName);
  void loadUniforms(const string& fileName);

  std::unordered_map<std::string, shared_ptr<ShaderProgram>>& getShaders() override;
  std::shared_ptr<ShaderProgram> getShader(const std::string& shaderName) override;
  std::shared_ptr<WaveInterface> getWaveInterface() override;
  std::vector<std::shared_ptr<Wave>>& getWaves() override;


  void setWaveParameter(const std::string& key, const ApiValue& value) override;
  void setApi(std::shared_ptr<ApiAdapter> api) override;
  std::shared_ptr<ApiAdapter> getApi() override;
  void setAnimator(std::shared_ptr<UniformAnimator> animator) override;
  std::shared_ptr<UniformAnimator> getAnimator() override;

  private:
  void loadJSON(const string& fileName, json& data)const;
  void loadCamera(const string& fileName);
  void loadShaders(const string& fileName);
  void loadLight(const string& fileName);
  void loadMesh(const string& fileName);
  void loadScene(const string& fileName);
  void loadGenerator(const string& fileName);
  void loadAPISettings(const string& fileName);

  std::weak_ptr<ApiAdapter> api;
  std::weak_ptr<UniformAnimator> animator;
  shared_ptr<ShaderProgram> buildShader(json& j, const string& name, vec4& color);

  std::unordered_map<std::string, shared_ptr<ShaderProgram> > shaders;
};

#endif
