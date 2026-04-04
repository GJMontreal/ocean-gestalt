#include "OceanGestalt.hpp"

#include "Configuration.hpp"
#include "Ocean.hpp"
#include "Uniforms.hpp"
#include "asset.hpp"
#include "glError.hpp"
#include "GltfModel.hpp"
#include "Prop.hpp"
#include "Skybox.hpp"
#include "TextRenderer.hpp"
#include "GerstnerWave.hpp"
#include "HeightMapGenerator.hpp"
#include "WaveGenerator.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
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
  
constexpr float TIME_WRAP_WINDOW = 86400.00f;
constexpr double TEXT_VISIBLE_TIME = 200;

// our application should keep a reference to the OceanApi
OceanGestalt::OceanGestalt() : Application() {
  configuration = std::make_shared<Configuration>(
      CONFIGURATION_DIR "scene.json", CONFIGURATION_DIR "shader.json",
      CONFIGURATION_DIR "generator.json", CONFIGURATION_DIR "api.json");
  buildScene();
 
  glEnable(GL_BLEND); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// Uniform buffers aren't supported by GLES
#ifndef __EMSCRIPTEN__
  initUniformBuffers();
#endif
  currentElement = sceneElements.begin(); 
  configuration->textRenderer->setShader(configuration->getShader("text"));
#ifdef DEBUG_GL
  glDumpTextureBindings();
#endif

  surfAudio = std::make_shared<SurfAudio>();
  doOnReady([audio = this->surfAudio]{audio->start();});

  onRender([audio = this->surfAudio, waves = configuration->getWaves(),
            camera = this->camera, running = this->isRunning](float time) {
    if (!running) {
      return;
    }
    auto position = camera->getPosition();
    audio->generateSurf(waves, position, camera->getYaw(), time);
  });
}

void OceanGestalt::buildScene() {
  this->camera = configuration->camera;
  this->camera->setConfiguration(configuration);
  this->camera->getMoveable().movementSpeed = 10.f;
  this->camera->getMoveable().setIsFloating(true);

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
  ocean->setIfShouldDrawWireframe(false);
  sceneElements.emplace_back(SceneElement{"waves",ocean,std::nullopt});
  
  for (auto& cfg : configuration->sceneModels) {
    auto mesh = std::make_shared<GltfModel>(
        std::string(MODEL_DIR) + cfg.file, cfg.position, configuration);
    mesh->setIfShouldDrawMesh(true);
    mesh->setScale(cfg.scale);
    mesh->setRotation(cfg.rotation);
    if (!cfg.shader.empty()) {
      if (auto shader = configuration->getShader(cfg.shader))
        mesh->setMeshShader(shader);
    }
#ifndef __EMSCRIPTEN__
    mesh->setNormalShader(configuration->getShader("drawable_normal"));
#endif
    auto prop = std::make_shared<Prop>(mesh, cfg.position, configuration);
    prop->getMoveable().setIsFloating(cfg.floating);
    prop->getMoveable().setIsTethered(cfg.tethered);
    prop->setRightingWeight(cfg.rightingWeight);
    prop->setSpinParameters(cfg.spinTorqueScale, cfg.angularDamping, cfg.torsionalStiffness);
    sceneElements.emplace_back(SceneElement{cfg.name, mesh, prop});
  }

  skybox = std::make_shared<Skybox>(configuration);

  reflectionPass = std::make_unique<ReflectionPass>(configuration->reflectionSize, configuration->reflectionSize);
  shadowPass = std::make_unique<ShadowPass>(configuration->shadowSize);
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

  auto uniformTime = static_cast<float>(std::fmod(elapsedTime, TIME_WRAP_WINDOW));

  projection = glm::perspective(glm::radians(getCamera()->getZoom()),
                                getWindowRatio(), 0.1f, 200.f);

  view = camera->getViewMatrix(elapsedTime);
  
  Uniforms uniforms{.projection = projection, .view = view, .time = uniformTime};

#ifndef __EMSCRIPTEN__
  setUniformBuffers(projection, view);
#endif

  // --- Shadow pre-pass ---
  {
    glm::vec3 lightPos = configuration->light->getPosition();
    float halfSize = configuration->meshSize * 0.5f;
    glm::mat4 lightProj = glm::ortho(-halfSize, halfSize, -halfSize, halfSize, 0.1f, 50.0f);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProj * lightView;

#ifndef __EMSCRIPTEN__
    setUniformBuffers(lightProj, lightView);
#endif
    shadowPass->beginCapture();
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 4.0f);

    Uniforms shadowUniforms{.projection = lightProj, .view = lightView, .time = uniformTime};
    for (auto& element : sceneElements) {
      if (element.name == "waves" || element.name == "camera") continue;
      if (element.drawable) {
        (*element.drawable)->draw(shadowUniforms);
      }
    }

    glDisable(GL_POLYGON_OFFSET_FILL);
    shadowPass->endCapture(getWidth(), getHeight());
#ifndef __EMSCRIPTEN__
    setUniformBuffers(projection, view);
#endif

    uniforms.lightSpaceMatrix = lightSpaceMatrix;
    uniforms.shadowTexture = shadowPass->getDepthTextureID();
  }

  // --- Reflection pre-pass ---
  {
    glm::mat4 reflectY = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f));
    glm::mat4 reflectedView = view * reflectY;
    glm::mat4 reflectionMatrix = projection * reflectedView;

#ifndef __EMSCRIPTEN__
    setUniformBuffers(projection, reflectedView);
#endif
    reflectionPass->beginCapture();
#ifndef __EMSCRIPTEN__
    glEnable(GL_CLIP_DISTANCE0);
#endif
    glFrontFace(GL_CW);

    Uniforms reflUniforms{.projection = projection,
                          .view = reflectedView,
                          .time = uniformTime,
                          .isReflectionPass = 1};
    
    skybox->draw(reflUniforms);

    for (auto& element : sceneElements) {
      if (element.name == "waves") continue;
      if (element.drawable) {
        (*element.drawable)->draw(reflUniforms);
      }
    }
  
    glFrontFace(GL_CCW);
#ifndef __EMSCRIPTEN__
    glDisable(GL_CLIP_DISTANCE0);
#endif
    reflectionPass->endCapture(getWidth(), getHeight());
#ifndef __EMSCRIPTEN__
    setUniformBuffers(projection, view);
#endif

    uniforms.reflectionMatrix = reflectionMatrix;
    uniforms.reflectionTexture = reflectionPass->getTextureID();
  }

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
  // we aren't getting in here enough to worry about optimizing this, yet
  // be sure to update the projection matrix if the window has changed size
  auto screenHeight = static_cast<float>(getHeight());
  configuration->textRenderer->setScreenHeight(screenHeight);
  configuration->textRenderer->setProjection(glm::ortho(0.0f, static_cast<float>(getWidth()),0.0f,screenHeight) );

  configuration->textRenderer->drawBegin();
  auto textYLocation = screenHeight - 30.f;
  auto textColor = glm::vec4(1.0f);
  
  auto textSize = 300.f; 

  configuration->textRenderer->drawText(
      fps.getFPSString(), {10.0f,  textYLocation, 0.0f}, textColor, textSize);
  configuration->textRenderer->drawText(currentElement->name,
                                        {200.0f,  textYLocation, 0.0f},
                                        textColor, textSize);
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
  muteAudio(!isRunning); 
}

void OceanGestalt::toggleWireframe() {
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

// TODO: this should apply to any drawable / moveable
void OceanGestalt::toggleFloatingCamera() {
  if( auto moveable = currentElement->moveable){
    (*moveable)->getMoveable().setIsFloating(!(*moveable)->getMoveable().getIsFloating());
  }
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
      std::cout << "Loading uniforms" << std::endl;
      configuration->loadUniforms(CONFIGURATION_DIR "uniforms_min.json");
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
  auto api = this->configuration->getApi();
  WaveGenerator(configuration, api);
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
    glfwGetWindowSize(window, &restorationWindowWidth, &restorationWindowHeight);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    monitor = glfwGetPrimaryMonitor();
  } else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    monitor = nullptr;
  }
  glfwSetWindowMonitor(window, monitor, windowXPos, windowYPos, restorationWindowWidth,
                       restorationWindowHeight, GLFW_DONT_CARE);
}

void OceanGestalt::doOnReady(const std::function<void()>& callback){ 
  onReadyCallbacks.push_back(callback);
}

void OceanGestalt::onRender(const std::function<void(float)>& callback){
  renderThreadCallbacks.emplace_back(callback);
}

void OceanGestalt::pauseSimulation(bool){
  toggleSimulation();
} 

void OceanGestalt::muteAudio(bool mute) {
  mute ? surfAudio->stop() : surfAudio->start();
}
