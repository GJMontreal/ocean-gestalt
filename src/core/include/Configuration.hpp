#ifndef __CONFIGURATION_HPP
#define __CONFIGURATION_HPP

#include "Wave.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Shader.hpp"
#include "AppContextInterface.hpp"
#include "ApiValue.hpp"

#include <memory.h>
#include <nlohmann/json.hpp>
#include <string>

using nlohmann::json;
using glm::vec3;
using glm::vec4;
using std::shared_ptr;
using std::vector;
using std::string;


class Configuration: public AppContextInterface {
 public:
  explicit Configuration(const string& environment, 
    const string& shader, 
    const string &generator,
    const string& api);  //why is api a string here?

  void setInitialUniformState(const ApiAdapter& api) override;
  vector<shared_ptr<Wave>> waves;

  shared_ptr<Camera> camera;
  
  shared_ptr<Light> light;

  shared_ptr<ShaderProgram> meshShader;  
  shared_ptr<ShaderProgram> wireframeShader;
  shared_ptr<ShaderProgram> normalShader;
  
  int meshSize;
  int meshSubdivisions;
  
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
  
  void setApi(std::shared_ptr<ApiAdapter> api) override;

  private:
  void loadJSON(const string& fileName, json& data)const;
  void loadWaves(const string& fileName);
  void loadCamera(const string& fileName);
  void loadShaders(const string& fileName);
  void loadLight(const string& fileName);
  void loadMesh(const string& fileName);
  void loadGenerator(const string& fileName);
  void loadAPISettings(const string& fileName);

  void setInitialWaveUniforms(const ApiAdapter& api) const;

  std::weak_ptr<ApiAdapter> api;
  shared_ptr<ShaderProgram> buildShader(json& j, const string& name, vec4& color);

  std::unordered_map<std::string, shared_ptr<ShaderProgram> > shaders;
};

#endif
