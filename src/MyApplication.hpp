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

class MyApplication : public Application {
 public:
  MyApplication();
 
 protected:
  void loop() override;
  void toggleNormalDisplay() override;
  void toggleSimulation() override;
  void toggleWireframe() override;

 private:
  // I don't think we use application level shaders
  // shader
  Shader vertexShader;
  Shader fragmentShader;
  ShaderProgram shaderProgram;

  // shader matrix uniform
  glm::mat4 projection = glm::mat4(1.0);
  glm::mat4 view = glm::mat4(1.0);
  GLuint uboMatrices;

  std::vector<Model*> models;

};

#endif  // OPENGL_CMAKE_SKELETON_MYAPPLICATION
