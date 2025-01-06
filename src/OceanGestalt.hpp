/**
 * OceanGestalt.hpp skeleton
 * Contributors:
 *      * Geoffrey Jones
 *      * Arthur Sonzogni
 * Licence:
 *      * MIT
 */

#ifndef __OCEAN_GESTALT_HPP
#define __OCEAN_GESTALT_HPP

#include "Application.hpp"
#include "Configuration.hpp"
#include "InputProcessor.hpp"
#include "Shader.hpp"
#include "Model.hpp"
#include "WaveUI.hpp"
#include "Moveable.hpp"

#include <map>

using std::unique_ptr;
using std::map;
using std::string;
using std::vector;

using glm::mat4;

class OceanGestalt : public Application, public Updatable{
 public:
  OceanGestalt();
 
 protected:
  void loop() override;
  void processInput(GLFWwindow *window, float deltaTime) override;
  void toggleNormalDisplay() override;
  void toggleSimulation() override;
  void toggleWireframe() override;
  void toggleMesh() override;

 private:
  // shader matrix uniform
  glm::mat4 projection = mat4(1.0);
  glm::mat4 view = mat4(1.0);

  GLuint uboMatrices;

  shared_ptr<Configuration> configuration;
  shared_ptr<Moveable> moveable;
  unique_ptr<WaveUI> waveUI;
  vector<Model*> models;

  // I think this next is unused
  // map<string, ShaderProgram*> shaderPrograms;

  void initUniformBuffers();
  void setUniformBuffers(mat4& projection, mat4& view) const;
  
  void updateWaves() const;
};

#endif
