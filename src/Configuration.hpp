#ifndef __CONFIGURATION_HPP
#define __CONFIGURATION_HPP

#include "Wave.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Shader.hpp"

#include <memory.h>
#include <nlohmann/json.hpp>
#include <string>

using nlohmann::json;
using glm::vec3;
using glm::vec4;
using std::shared_ptr;
using std::vector;
using std::string;


class Configuration {
 public:
  explicit Configuration(const string& environment, const string& shader, const string &generator);

  vector<shared_ptr<Wave>> waves;

  shared_ptr<Camera> camera;
  
  shared_ptr<Light> light;

  shared_ptr<ShaderProgram> meshShader;  
  shared_ptr<ShaderProgram> wireframeShader;
  shared_ptr<ShaderProgram> normalShader;
  
  int meshSize;

  float medianWavelength;
  float medianAmplitude;
  float directionalVariance;
  float stdDeviation;
  
  vec4 meshColor;
  vec4 wireframeColor;
  vec4 normalColor;

  void save(const string& fileName);

  private:
  void loadJSON(const string& fileName, json& data)const;
  void loadWaves(const string& fileName);
  void loadCamera(const string& fileName);
  void loadShaders(const string& fileName);
  void loadLight(const string& fileName);
  void loadMeshSize(const string& fileName);
  void loadGenerator(const string& fileName);

  shared_ptr<ShaderProgram> buildShader(json& j, const string& name, vec4& color);
};

#endif
