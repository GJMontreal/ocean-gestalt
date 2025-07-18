#include "OceanGestalt.hpp"

#include "Configuration.hpp"
#include "Ocean.hpp"
#include "Uniforms.hpp"
#include "asset.hpp"
#include "glError.hpp"
#include "Buoy.hpp"
#include "Skybox.hpp"
#include "TextRenderer.hpp"
#include "GerstnerWave.hpp"
#include "HeightMapGenerator.hpp"
#include "WaveGenerator.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <thread>
#include <memory>
#include <nlohmann/json.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using std::cout;
using std::endl;
  
constexpr float TIME_WRAP_WINDOW = 10000.00f;
constexpr double TEXT_VISIBLE_TIME = 200;

// our application should keep a reference to the OceanApi
OceanGestalt::OceanGestalt() : Application() {
  configuration = std::make_shared<Configuration>(
      CONFIGURATION_DIR "environment.json", CONFIGURATION_DIR "shader.json",
      CONFIGURATION_DIR "generator.json", CONFIGURATION_DIR "api.json");
  buildScene();
 
  glEnable(GL_BLEND); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// Uniform buffers aren't supported by GLES
#ifndef __EMSCRIPTEN__
  initUniformBuffers();
#endif

  // onRender([&]{surfAudio->setFoamLevel(float foam);});
  currentElement = sceneElements.begin(); //we should have selectable, which could be drawable moveable one or the other or both

  auto screenHeight = static_cast<float>(getHeight());
  configuration->textRenderer->setScreenHeight(screenHeight);
  configuration->textRenderer->setProjection(glm::ortho(0.0f, static_cast<float>(getWidth()),0.0f,screenHeight) );
  configuration->textRenderer->setShader(configuration->getShader("text"));
#ifdef DEBUG_GL
  glDumpTextureBindings();
#endif

  surfAudio = std::make_shared<SurfAudio>();
  doOnReady([audio = this->surfAudio]{audio->start();});

  onRender([audio=this->surfAudio,waves = configuration->getWaves(), camera = this->camera](float time) {
    auto position = camera->getPosition();
    audio->generateSurf(waves, position, camera->getYaw(), time);
  });
}

void OceanGestalt::buildScene() {
  this->camera = configuration->camera;
  this->camera->setConfiguration(configuration);
  this->camera->getMoveable().movementSpeed = 10.f;
  this->camera->setIsFloating(true);

  sceneElements.emplace_back(SceneElement{"camera",std::nullopt,this->camera});
  
  this->light = std::make_shared<Light>(configuration->lightPosition, configuration);
  configuration->light = light;
  auto lightDrawable = this->light->getDrawable();

  auto drawableMeshShader = configuration->getShader("drawable_mesh");
#ifndef __EMSCRIPTEN__
  auto drawableNormalShader = configuration->getShader("drawable_normal");
#endif
  lightDrawable->setShader(drawableMeshShader);
  lightDrawable->setIfShouldDrawMesh(false);
#ifndef __EMSCRIPTEN__
  lightDrawable->setNormalShader(drawableNormalShader);
#endif
  sceneElements.emplace_back(SceneElement{"light",lightDrawable, this->light});  //TODO: light should have a moveable, not be a moveable

  auto ocean = std::make_shared<Ocean>(configuration);
  ocean->setIfShouldDrawMesh(true);
  ocean->setIfShouldDrawLines(true);
  sceneElements.emplace_back(SceneElement{"waves",ocean,std::nullopt});
  
  auto buoy = std::make_shared<Buoy>(glm::vec3(5.0f,0.f,5.0f), configuration);
  auto buoyDrawable = buoy->getDrawable();
  buoyDrawable->setShader(configuration->getShader("buoy_mesh"));
  buoyDrawable->setIfShouldDrawMesh(true);
#ifndef __EMSCRIPTEN__
  buoyDrawable->setNormalShader(drawableNormalShader);
#endif
  sceneElements.emplace_back(SceneElement{"buoy",buoyDrawable,buoy});
}

void OceanGestalt::runOnce(){
for(auto const& cb : onReadyCallbacks){
      cb();
  }
  onReadyCallbacks.clear();
}

void OceanGestalt::loop() {
  // exit on window close button pressed
  if (glfwWindowShouldClose(getWindow()))
    exit();

  fps.update();

  auto time = glfwGetTime();
  auto interval = time - lastTime;
  lastTime = time;

  //update our time if the simulation is running
  if (isRunning) {
    elapsedTime += interval;
  }

  for(auto const& cb : renderThreadCallbacks){
      cb(elapsedTime);
  }

  double anchor = std::floor(elapsedTime/TIME_WRAP_WINDOW) * TIME_WRAP_WINDOW;
  auto uniformTime = static_cast<float>(elapsedTime - anchor);

  projection = glm::perspective(glm::radians(getCamera()->getZoom()),
                                getWindowRatio(), 0.1f, 200.f);

  view = camera->getViewMatrix(elapsedTime);
  
  Uniforms uniforms{.projection = projection, .view = view, .time = uniformTime};

#ifndef __EMSCRIPTEN__
  setUniformBuffers(projection, view);
#endif

  // clear
  glClear(GL_COLOR_BUFFER_BIT);
 
  glClearColor(0.0f, 0.05f, 0.1f,
               1.0f);  // TODO: we should set this in the environment
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for( auto element : sceneElements){
    if(element.drawable){
      (*element.drawable)->draw(uniforms);
    }
  }

  if(shouldRenderText){
    renderText();
    if( ++textVisibleTime > TEXT_VISIBLE_TIME){
      textVisibleTime = 0;
      shouldRenderText = false;
    }
  }
}

void OceanGestalt::renderText() {
  configuration->textRenderer->drawBegin();
  configuration->textRenderer->drawText(
      fps.getFPSString(), {10.0f, 10.0f, 0.0f}, {1.f, 0.f, 0.f, 1.f}, 100.0f);
  configuration->textRenderer->drawText(currentElement->name,
                                        {200.0f, 10.0f, 0.0f},
                                        {1.f, 0.f, 0.f, 1.f}, 100.0f);
  configuration->textRenderer->render();
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
  if (auto drawable = currentElement->drawable) {
    (*drawable)->setIfShouldDrawNormals(!(*drawable)->getIfShouldDrawNormals());
  }
}

void OceanGestalt::toggleSimulation() {
  std::cout << "Toggle simulation" << std::endl;
  isRunning = !isRunning;
}

void OceanGestalt::toggleWireframe() {
  std::cout << "Toggle wireframe" << std::endl;
    if (auto drawable = currentElement->drawable) {
    (*drawable)->setIfShouldDrawWireframe(!(*drawable)->getIfShouldDrawWireframe());
  }
}

void OceanGestalt::toggleMesh() {
  std::cout << "Toggle mesh" << std::endl;
  if (auto drawable = currentElement->drawable) {
    (*drawable)->setIfShouldDrawMesh(!(*drawable)->getIfShouldDrawMesh());
  }
}

void OceanGestalt::toggleDrawTriangles() {
  std::cout << "Toggle triangles" << std::endl;

}

void OceanGestalt::toggleDrawLines() {
  std::cout << "Toggle lines" << std::endl;
  if (auto drawable = currentElement->drawable) {
    (*drawable)->setIfShouldDrawLines(!(*drawable)->getIfShouldDrawLines());
  }
}

void OceanGestalt::toggleFloatingCamera() {
  camera->setIsFloating(!camera->getIsFloating());
}

void OceanGestalt::toggleDisplayText() {
  shouldRenderText = true;
}

void OceanGestalt::selectNextElement() {
  // temporarily enable text rendering
  shouldRenderText = true;
  if (++currentElement == sceneElements.end()) {
    currentElement = sceneElements.begin();
  }
}

void OceanGestalt::loadUniforms() {
#ifndef __EMSCRIPTEN__
    std::thread([this] {
      configuration->loadUniforms(CONFIGURATION_DIR "uniforms.json");
    }).detach();
#else
    auto* that = this;
  emscripten_async_call([](void* arg) {
    auto* self = static_cast<OceanGestalt*>(arg);
    std::cout << "Loading uniforms" << std::endl;
    self->configuration->loadUniforms(CONFIGURATION_DIR "uniforms.json");
  }, that, 0);  // delay = 0 ms
#endif
}

void OceanGestalt::generateUniforms() {
#ifndef __EMSCRIPTEN__
   auto api = this->configuration->getApi();
    glm::vec2 dir{ randf(-1.0f,1.0f),randf(-1.0f,1.0f)};
    Wind wind{dir,randf(0.1f,100.f)};
    WaveGenerator(configuration->waves.size(), wind, api);
#endif
}

void OceanGestalt::dumpUniforms() {
  configuration->dumpUniforms(CONFIGURATION_DIR "/uniforms.json");
}

void OceanGestalt::processInput(GLFWwindow* window, float deltaTime) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  //I don't recall why there's this extra layer of indirection around moveables. Shit happens
  if(auto currentMoveable = currentElement->moveable){
  auto& moveable = (*currentMoveable)->getMoveable();
  for (auto& [key, direction] : movementKeys) {
    if (glfwGetKey(window, key) == GLFW_PRESS) {
      moveable.ProcessKeyboard( direction, deltaTime);
    }
  }
  }
  

  for (const auto& pair : keyBindings.getKeyBindings()) {
    executeIfPressed(window, pair.first,
                     [action = pair.second, this]() { action(this); });
  }
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

void OceanGestalt::onRender(const std::function<void(float)>& callback){
  renderThreadCallbacks.push_back(callback);
}

void OceanGestalt::pauseSimulation(bool){
  toggleSimulation();
} 

