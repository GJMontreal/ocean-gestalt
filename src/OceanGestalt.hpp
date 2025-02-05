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
#include "KeyExecutable.hpp"
#include "Shader.hpp"
#include "Model.hpp"
#include "WaveUI.hpp"
#include "Moveable.hpp"
#include "KeyExecutable.hpp"


using std::unique_ptr;
using std::string;
using std::vector;

using glm::mat4;

class OceanGestalt : public Application, public Updatable, public KeyExecutable{
 public:
  OceanGestalt();
 
 protected:
  void setUIDelegate() override;
  void loop() override;
  void processInput(GLFWwindow *window, float deltaTime) override;

 private:
  // for restoring the window from fullscreen
  int windowXPos;
  int windowYPos;
  int windowWidth;
  int windowHeight;

  // shader matrix uniform
  glm::mat4 projection = mat4(1.0);
  glm::mat4 view = mat4(1.0);

  GLuint uboMatrices;

  shared_ptr<Configuration> configuration;
  shared_ptr<Moveable> moveable;
  unique_ptr<WaveUI> waveUI;
  vector<Model*> models;

  void toggleNormalDisplay();
  void toggleSimulation();
  void toggleWireframe();
  void toggleMesh();
  void toggleDrawTriangles();
  void toggleDrawLines();

  void toggleFullscreen(GLFWwindow*);

  void initUniformBuffers();
  void setUniformBuffers(mat4& projection, mat4& view) const;
  
  void updateWaves() const;
};

#endif
