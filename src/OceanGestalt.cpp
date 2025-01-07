#include "OceanGestalt.hpp"

#include "Configuration.hpp"
#include "Ocean.hpp"
#include "Uniforms.hpp"
#include "asset.hpp"
#include "glError.hpp"
#include "WaveGenerator.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/gtc/type_ptr.hpp"

#include <iostream>

#include <nlohmann/json.hpp>

using std::cout;
using std::endl;


// TODO: move this template elsewhere
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

OceanGestalt::OceanGestalt()
    : Application()
{
  // TODO: pass in file names as optional command line arguments
  auto config = std::make_shared<Configuration>(CONFIGURATION_DIR "environment.json",
  CONFIGURATION_DIR "shader.json",CONFIGURATION_DIR "generator.json");
  this->camera = config->camera; 
  this->moveable = this->camera;
  this->light = config->light;
  
  configuration = config;
  models.push_back(new Ocean(config));
 
  waveUI = make_unique<WaveUI>(config->waves,(Updatable*)(this));
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);  // Not certain what our blend mode should be?

// Uniform buffers aren't supported by GLES
#ifndef __EMSCRIPTEN__
  initUniformBuffers();
#endif
}

void OceanGestalt::loop() {
  if(wavesNeedUpdate){
    wavesNeedUpdate = false;
    updateWaves();
  }
  // exit on window close button pressed
  if (glfwWindowShouldClose(getWindow()))
    exit();

  projection = glm::perspective(glm::radians(getCamera()->Zoom),
                                getWindowRatio(), 0.1f, 50.f);

  view = camera->GetViewMatrix();
  
  Uniforms uniforms{.projection = projection, .view = view};

#ifndef __EMSCRIPTEN__
 setUniformBuffers(projection, view);
#endif

  // clear
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for (Model* model : models) {
    model->draw(uniforms);
  }
}

void OceanGestalt::initUniformBuffers() {
  glGenBuffers(1, &uboMatrices);
  glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr,
               GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0,
                    2 * sizeof(glm::mat4));
  glCheckError(__FILE__, __LINE__);
}

void OceanGestalt::setUniformBuffers(mat4& projection, mat4& view) const{
  glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                  glm::value_ptr(projection));
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                  glm::value_ptr(view));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OceanGestalt::toggleNormalDisplay() {
  std::cout << "Toggle normals" << std::endl;
  for (Model* model : models) {
    model->toggleDrawNormals();
  }
}

void OceanGestalt::toggleSimulation() {
  std::cout << "Toggle simulation" << std::endl;
  for (Model* model : models) {
    model->toggleRunning();
  }
}

void OceanGestalt::toggleWireframe() {
  std::cout << "Toggle wireframe" << std::endl;
  for (Model* model : models) {
    model->toggleDrawWireframe();
  }
}

void OceanGestalt::toggleMesh() {
  std::cout << "Toggle mesh" << std::endl;
  for (Model* model : models) {
    model->toggleDrawMesh();
  }
}

void OceanGestalt::processInput(GLFWwindow* window, float deltaTime) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  
  // TODO:
  // moveable->processInput(window, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
    moveable->ProcessKeyboard(Movement::FORWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    moveable->ProcessKeyboard(Movement::BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    moveable->ProcessKeyboard(Movement::LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    moveable->ProcessKeyboard(Movement::RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    moveable->ProcessKeyboard(Movement::UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    moveable->ProcessKeyboard(Movement::DOWN, deltaTime);

  waveUI->processInput(window, deltaTime);

  // These are problematic because they might get called many times
  // for each; save the state and only call the function when it changes
  // to release
  // Toggle between moving the camera and light
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
    if (keyPressState[GLFW_KEY_C] == GLFW_RELEASE) {
      if(moveable == configuration->camera){
        moveable = configuration->light;
      }else {
        moveable = configuration->camera;
      }
      keyPressState[GLFW_KEY_C] = GLFW_PRESS;
    }
  } else {
    keyPressState[GLFW_KEY_C] = GLFW_RELEASE;
  }
  
  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
    if (keyPressState[GLFW_KEY_N] == GLFW_RELEASE) {
      toggleNormalDisplay();
      keyPressState[GLFW_KEY_N] = GLFW_PRESS;
    }
  } else {
    keyPressState[GLFW_KEY_N] = GLFW_RELEASE;
  }

  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    if(keyPressState[GLFW_KEY_P] == GLFW_RELEASE){
      toggleSimulation();
      keyPressState[GLFW_KEY_P] = GLFW_PRESS;
    }
  }else{
    keyPressState[GLFW_KEY_P] = GLFW_RELEASE;
  }

  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
    if(keyPressState[GLFW_KEY_L] == GLFW_RELEASE){
      toggleWireframe();
      keyPressState[GLFW_KEY_L] = GLFW_PRESS;
    }
  }else{
    keyPressState[GLFW_KEY_L] = GLFW_RELEASE;
  }

  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
    if(keyPressState[GLFW_KEY_M] == GLFW_RELEASE){
      toggleMesh();
      keyPressState[GLFW_KEY_M] = GLFW_PRESS;
    }
  }else{
    keyPressState[GLFW_KEY_M] = GLFW_RELEASE;
  }

// could this be written as a template?
  if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
    if(keyPressState[GLFW_KEY_Z] == GLFW_RELEASE){
      configuration->save(CONFIGURATION_DIR "/output.json");
      keyPressState[GLFW_KEY_Z] = GLFW_PRESS;
    }
  }else{
    keyPressState[GLFW_KEY_Z] = GLFW_RELEASE;
  }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    if(keyPressState[GLFW_KEY_R] == GLFW_RELEASE){
      WaveGenerator(configuration->waves, configuration->stdDeviation,
                    configuration->medianWavelength,
                    configuration->medianAmplitude);
      wavesNeedUpdate = true;
      keyPressState[GLFW_KEY_R] = GLFW_PRESS;
    }
  }else{
    keyPressState[GLFW_KEY_R] = GLFW_RELEASE;
  }
}

void OceanGestalt::updateWaves() const{
  for (Model* model : models) {
    model->updateShaderUniforms();
  }
}