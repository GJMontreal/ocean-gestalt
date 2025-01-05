#include "MyApplication.hpp"

#include "Configuration.hpp"
#include "Ocean.hpp"
#include "Uniforms.hpp"
#include "asset.hpp"
#include "glError.hpp"
#include "Debug.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <vector>

#include <nlohmann/json.hpp>

using std::cout;
using std::endl;


// TODO: move this template elsewhere
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

MyApplication::MyApplication()
    : Application()
{
  auto config = std::make_shared<Configuration>(CONFIGURATION_DIR "/config.json");
  this->camera = config->camera;  // TODO: we don't need this additional pointer since we have the configuration
  configuration = config;
  models.push_back(new Ocean(20,config));
  // waveUI = new WaveUI(config->waves);
 
  waveUI = make_unique<WaveUI>(config->waves,(Updatable*)(this));
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);  // Not certain what our blend mode should be?

// Uniform buffers aren't supported in GLES
#ifndef __EMSCRIPTEN__
  glGenBuffers(1, &uboMatrices);
  glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr,
               GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0,
                    2 * sizeof(glm::mat4));
  glCheckError(__FILE__, __LINE__);
#endif
}

void MyApplication::loop() {
  if(wavesNeedUpdate){
    wavesNeedUpdate = false;
    updateWaves();
  }
  // exit on window close button pressed
  if (glfwWindowShouldClose(getWindow()))
    exit();

  projection = glm::perspective(glm::radians(getCamera()->Zoom),
                                getWindowRatio(), 0.1f, 150.f);

  view = camera->GetViewMatrix();
  
  Uniforms uniforms{.projection = projection, .view = view};
#ifndef __EMSCRIPTEN__
  // Set common shader uniforms
  glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                  glm::value_ptr(projection));
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                  glm::value_ptr(view));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
#endif

  // clear
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for (Model* model : models) {
    model->draw(uniforms);
  }
}

void MyApplication::toggleNormalDisplay() {
  std::cout << "Toggle normals" << std::endl;
  for (Model* model : models) {
    model->toggleDrawNormals();
  }
}

void MyApplication::toggleSimulation() {
  std::cout << "Toggle simulation" << std::endl;
  for (Model* model : models) {
    model->toggleRunning();
  }
}

void MyApplication::toggleWireframe() {
  std::cout << "Toggle wireframe" << std::endl;
  for (Model* model : models) {
    model->toggleDrawWireframe();
  }
}

void MyApplication::toggleMesh() {
  std::cout << "Toggle mesh" << std::endl;
  for (Model* model : models) {
    model->toggleDrawMesh();
  }
}

void MyApplication::processInput(GLFWwindow* window, float deltaTime) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera->ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera->ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera->ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera->ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    camera->ProcessKeyboard(Camera_Movement::UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    camera->ProcessKeyboard(Camera_Movement::DOWN, deltaTime);

  waveUI->processInput(window, deltaTime);
  // These are problematic because they might get called many times
  // for each; save the state and only call the function when it changes
  // to release
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
    if (keyPressState[GLFW_KEY_C] == GLFW_RELEASE) {
      dumpCameraMatrices();
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
}

void MyApplication::dumpCameraMatrices(){
  dumpVector("Front", camera->Front);
  dumpVector("Position", camera->Position);
  dumpVector("Up", camera->Up);
  std::cout << "Yaw " << camera->Yaw << std::endl;
  std::cout << "Pitch " << camera->Pitch << std::endl;
}

void MyApplication::updateWaves(){
  for (Model* model : models) {
    model->updateShaderUniforms();
  }
}