/**
 * MyApplication.hpp skeleton
 * Contributors:
 *      * Arthur Sonzogni
 * Licence:
 *      * MIT
 */

#ifndef OPENGL_CMAKE_SKELETON_MYAPPLICATION
#define OPENGL_CMAKE_SKELETON_MYAPPLICATION

#include "Application.hpp"
#include "Shader.hpp"
#include "Model.hpp"
#include "WaveUI.hpp"

#include <map>

using namespace std;

class MyApplication : public Application {
 public:
  MyApplication();
 
 protected:
  void loop() override;
  void processInput(GLFWwindow *window, float deltaTime) override;
  void toggleNormalDisplay() override;
  void toggleSimulation() override;
  void toggleWireframe() override;
  void toggleMesh() override;

 private:
  // shader matrix uniform
  glm::mat4 projection = glm::mat4(1.0);
  glm::mat4 view = glm::mat4(1.0);
  GLuint uboMatrices;

  unique_ptr<WaveUI> waveUI;
  vector<Model*> models;
  map<string, ShaderProgram*> shaderPrograms;

  void dumpCameraMatrices();
};

#endif  // OPENGL_CMAKE_SKELETON_MYAPPLICATION
