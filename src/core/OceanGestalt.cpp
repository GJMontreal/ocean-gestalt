#include "OceanGestalt.hpp"

#include "Configuration.hpp"
#include "Ocean.hpp"
#include "Uniforms.hpp"
#include "WaveGenerator.hpp"
#include "asset.hpp"
#include "glError.hpp"
#include "Buoy.hpp"
#include "TextRenderer.hpp"
#include "GerstnerWave.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <thread>
#include <memory>
#include <nlohmann/json.hpp>

using std::cout;
using std::endl;

// our application should keep a reference to the OceanApi
OceanGestalt::OceanGestalt() : Application() {
  auto config = std::make_shared<Configuration>(
      CONFIGURATION_DIR "environment.json", CONFIGURATION_DIR "shader.json",
      CONFIGURATION_DIR "generator.json", CONFIGURATION_DIR "api.json");
  this->camera = config->camera;
  moveables.push_back(this->camera);

  this->light = std::make_shared<Light>(config->lightPosition, config);
  config->light = light;
  moveables.push_back(this->light);
  auto lightDrawable = this->light->getDrawable();

  auto drawableMeshShader = config->getShaders()["drawable_mesh"];
  auto drawableNormalShader = config->getShaders()["drawable_normal"];
  
  lightDrawable->setShader(drawableMeshShader);
  lightDrawable->setNormalShader(drawableNormalShader);
  drawables.push_back(this->light->getDrawable());

  configuration = config;
  auto ocean = std::make_shared<Ocean>(config);
  models.push_back(ocean.get()); // yeah, we don't want this
  drawables.push_back(ocean);

  auto buoy = std::make_shared<Buoy>(glm::vec3(0.7f,0.f,0.1f),config);
  auto buoyDrawable = buoy->getDrawable();
  buoyDrawable->setShader(config->getShaders()["buoy_mesh"]);
  buoyDrawable->setNormalShader(drawableNormalShader);
  drawables.push_back(buoyDrawable);
  moveables.push_back(buoy);

  waveUI = unique_ptr<WaveUI>(new WaveUI(config->waves));

  // glEnable(GL_BLEND);
  // glBlendFunc(GL_ONE, GL_ONE);  // Not certain what our blend mode should be?

// Uniform buffers aren't supported by GLES
#ifndef __EMSCRIPTEN__
  initUniformBuffers();
#endif
  
  auto textRenderer = std::make_shared<TextRenderer>(FONT_DIR 
  "FiraCode-Regular.ttf", 90);
  config->textRenderer = textRenderer;
  textRenderer->setShader(config->getShaders()["text"]);
  // textRenderer->setProjection(glm::ortho(0.0f, static_cast<float>(getWidth()),static_cast<float>(getHeight()),0.0f )); 
  textRenderer->setScreenHeight(static_cast<float>(getHeight()));
  textRenderer->setProjection(glm::ortho(0.0f, static_cast<float>(getWidth()),0.0f,static_cast<float>(getHeight()) ));
  // surfAudio = std::make_shared<SurfAudio>();
  // doOnReady([&]{surfAudio->start();});

  // onRender([&]{surfAudio->setFoamLevel(float foam);});
  currentMoveable = moveables.begin();
}

void OceanGestalt::setUIDelegate() {
  waveUI->updatable = weak_from_this();
}

void OceanGestalt::runOnce(){
for(auto const& cb : onReadyCallbacks){
      cb();
  }
  onReadyCallbacks.clear();
}

void OceanGestalt::loop() {

  for(auto const& cb : renderThreadCallbacks){
      cb();
  }

  // exit on window close button pressed
  if (glfwWindowShouldClose(getWindow()))
    exit();

  auto time = float(glfwGetTime());
  auto interval = time - lastTime;
  lastTime = time;

  //update our time if the simulation is running
  if (isRunning) {
    elapsedTime += (float)interval;
  }

  projection = glm::perspective(glm::radians(getCamera()->Zoom),
                                getWindowRatio(), 0.1f, 200.f);
  // view = camera->GetViewMatrix();
 

  // if we want to have the camera floating
  auto position = camera->getPosition();
  auto waveOffset = evaluateGerstnerWaves(configuration->getWaves(), glm::vec2(position.x,position.z),elapsedTime);
  view = glm::lookAt(position + waveOffset, position + waveOffset + camera->Front, camera->Up);
  
  Uniforms uniforms{.projection = projection, .view = view, .time = elapsedTime};

#ifndef __EMSCRIPTEN__
  setUniformBuffers(projection, view);
#endif

  // clear
  glClear(GL_COLOR_BUFFER_BIT);
  // glClearColor(0.0, 0.0, 0.0, 0.0);
  glClearColor(0.0f, 0.05f, 0.1f,
               1.0f);  // we should set this in the environment
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  // just until we get our text rendering working
  for( auto drawable : drawables){
    drawable->draw(uniforms);
  }

  glDisable(GL_DEPTH_TEST);
glDepthMask(GL_FALSE);
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//just until we get this working
configuration->textRenderer->drawBegin();
configuration->textRenderer->drawText("Hello there\nDo we get a newline?",glm::vec3(10.f,100.f,0.f),glm::vec4(1.0f,1.f,0.f,1.f), 64.f); //I think this size is in pixels
configuration->textRenderer->drawText("this should be a different color",glm::vec3(10.f,200.f,0.f),glm::vec4(0.f,1.f,0.f,1.f), 64.f);
configuration->textRenderer->render();

glEnable(GL_DEPTH_TEST);
glDepthMask(GL_TRUE);
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

void OceanGestalt::setUniformBuffers(mat4& projection, mat4& view) const {
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
  isRunning = !isRunning;
  // for (Model* model : models) {
  //   model->toggleRunning();
  // }

  // why are we doing this following?
  configuration->wireframeShader->activate();
  auto color = glm::vec4(0.5,0.5,0.5,1.0);
  configuration->wireframeShader->setUniform("lineColor",color);
  configuration->wireframeShader->deactivate();
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

void OceanGestalt::toggleDrawTriangles() {
  std::cout << "Toggle triangles" << std::endl;
  for (Model* model : models) {
    model->toggleDrawTriangles();
  }
}

void OceanGestalt::toggleDrawLines() {
  std::cout << "Toggle lines" << std::endl;
  for (Model* model : models) {
    model->toggleDrawLines();
  }
}

void OceanGestalt::dumpUniforms() {
  configuration->dumpUniforms(CONFIGURATION_DIR "/uniforms.json");
}

void OceanGestalt::processInput(GLFWwindow* window, float deltaTime) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  // TODO:
  // moveable->processInput(window, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    (*currentMoveable)->getMoveable().ProcessKeyboard(Movement::FORWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    (*currentMoveable)->getMoveable().ProcessKeyboard(Movement::BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    (*currentMoveable)->getMoveable().ProcessKeyboard(Movement::LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    (*currentMoveable)->getMoveable().ProcessKeyboard(Movement::RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    (*currentMoveable)->getMoveable().ProcessKeyboard(Movement::UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    (*currentMoveable)->getMoveable().ProcessKeyboard(Movement::DOWN, deltaTime);

  executeIfPressed(window, GLFW_KEY_G, [this]() {
    std::thread([this] {
      configuration->loadUniforms(CONFIGURATION_DIR "uniforms.json");
    }).detach();
  });

  waveUI->processInput(window, deltaTime);

  executeIfPressed(window, GLFW_KEY_C, [this]() {
    (*currentMoveable)->deactivate();
    ++currentMoveable;
    if(currentMoveable == moveables.end()){
      currentMoveable = moveables.begin();
    }
    (*currentMoveable)->activate();
    // cout << "activating " << (*currentMoveable)->getName() << std::endl;
  });

  executeIfPressed(window, GLFW_KEY_N, [this]() { toggleNormalDisplay(); });

  executeIfPressed(window, GLFW_KEY_P, [this]() { toggleSimulation(); });

  executeIfPressed(window, GLFW_KEY_M, [this]() { toggleMesh(); });

  executeIfPressed(window, GLFW_KEY_J, [this]() { toggleDrawTriangles(); });

  executeIfPressed(window, GLFW_KEY_K, [this]() { toggleDrawLines(); });

  executeIfPressed(window, GLFW_KEY_L, [this]() { toggleWireframe(); });

  executeIfPressed(window, GLFW_KEY_B, [this](){ dumpUniforms(); });

  executeIfPressed(window, GLFW_KEY_O, [this]() {
    configuration->save(CONFIGURATION_DIR "/output.json");
  });

  executeIfPressed(window, GLFW_KEY_R, [this]() {
    WaveGenerator(configuration->waves, configuration->stdDeviation,
                  configuration->medianWavelength,
                  configuration->medianAmplitude);
    wavesNeedUpdate = true;
  });

#ifndef __EMSCRIPTEN__
  executeIfPressed(window, GLFW_KEY_F,
                   [this, window]() { toggleFullscreen(window); });
#endif
}

void OceanGestalt::toggleFullscreen(GLFWwindow* window) {
  GLFWmonitor* monitor = glfwGetWindowMonitor(window);
  if (monitor == nullptr) {
    // save position and size
    glfwGetWindowPos(window, &windowXPos, &windowYPos);
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    monitor = glfwGetPrimaryMonitor();
  } else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    monitor = nullptr;
  }
  glfwSetWindowMonitor(window, monitor, windowXPos, windowYPos, windowWidth,
                       windowHeight, GLFW_DONT_CARE);
}

void OceanGestalt::doOnReady(const std::function<void()>& callback){ 
  onReadyCallbacks.push_back(callback);
}

void OceanGestalt::onRender(const std::function<void()>& callback){
  renderThreadCallbacks.push_back(callback);
}

void OceanGestalt::pauseSimulation(bool){
  toggleSimulation();
} 
