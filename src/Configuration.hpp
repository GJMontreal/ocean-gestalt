#ifndef __CONFIGURATION_HPP
#define __CONFIGURATION_HPP

#include "Wave.hpp"
#include "Camera.hpp"
#include "Shader.hpp"

#include <memory.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using vec3 = glm::vec3;

using namespace std;

class Configuration {
 public:
  explicit Configuration(const std::string& fileName);

  vector<shared_ptr<Wave>> waves;

  shared_ptr<Camera> camera;
  
  shared_ptr<ShaderProgram> meshShader;  
  shared_ptr<ShaderProgram> wireframeShader;
  shared_ptr<ShaderProgram> normalShader;
  
  vec3 lightPosition;

  void save(const string& fileName);

  private:
  int loadJSON(const string& fileName, json& data)const;
  int loadWaves(const string& fileName);
  int loadCamera(const string& fileName);
  void loadShaders(const string& fileName);
  int loadLight(const string& fileName);
};

#endif
