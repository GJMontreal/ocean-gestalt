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

  static const std::vector< std::pair<int, Movement>> movementKeys{
    {GLFW_KEY_W, Movement::FORWARD},
    {GLFW_KEY_S, Movement::BACKWARD},
    {GLFW_KEY_A, Movement::LEFT},
    {GLFW_KEY_D, Movement::RIGHT},
    {GLFW_KEY_SPACE, Movement::UP},
    {GLFW_KEY_LEFT_SHIFT, Movement::DOWN}
  };

  
// our application should keep a reference to the OceanApi
OceanGestalt::OceanGestalt() : Application() {
  auto config = std::make_shared<Configuration>(
      CONFIGURATION_DIR "environment.json", CONFIGURATION_DIR "shader.json",
      CONFIGURATION_DIR "generator.json", CONFIGURATION_DIR "api.json");
  this->camera = config->camera;
  this->camera->getMoveable().movementSpeed = 10.f;
  moveables.push_back(this->camera);

  this->light = std::make_shared<Light>(config->lightPosition, config);
  config->light = light;
  moveables.push_back(this->light);
  auto lightDrawable = this->light->getDrawable();

  auto drawableMeshShader = config->getShader("drawable_mesh");
#ifndef __EMSCRIPTEN__
  auto drawableNormalShader = config->getShader("drawable_normal");
#endif
  lightDrawable->setShader(drawableMeshShader);
#ifndef __EMSCRIPTEN__
  lightDrawable->setNormalShader(drawableNormalShader);
#endif
  drawables.push_back(this->light->getDrawable());

  configuration = config;
  auto ocean = std::make_shared<Ocean>(config);
  models.push_back(ocean.get()); // yeah, we don't want this ?
  drawables.push_back(ocean);

  auto buoy = std::make_shared<Buoy>(glm::vec3(5.0f,0.f,5.0f),config);
  auto buoyDrawable = buoy->getDrawable();
  buoyDrawable->setShader(config->getShader("buoy_mesh"));
#ifndef __EMSCRIPTEN__
  buoyDrawable->setNormalShader(drawableNormalShader);
#endif
  drawables.push_back(buoyDrawable);
  moveables.push_back(buoy);

  glEnable(GL_BLEND); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// Uniform buffers aren't supported by GLES
#ifndef __EMSCRIPTEN__
  initUniformBuffers();
#endif

  // onRender([&]{surfAudio->setFoamLevel(float foam);});
  currentMoveable = moveables.begin(); //we should have selectable, which could be drawable moveable one or the other or both

#ifdef DEBUG_GL
  glDumpTextureBindings();
#endif
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
  for(auto const& cb : renderThreadCallbacks){
      cb();
  }
  auto time = float(glfwGetTime());
  auto interval = time - lastTime;
  lastTime = time;

  //update our time if the simulation is running
  if (isRunning) {
    elapsedTime += (float)interval;
  }

  projection = glm::perspective(glm::radians(getCamera()->Zoom),
                                getWindowRatio(), 0.1f, 200.f);

  
  //TODO: move this into the camera
  if(floatingCamera){
    auto position = camera->getPosition();
    auto waveOffset = evaluateGerstnerWaves(configuration->getWaves(), glm::vec2(position.x,position.z),elapsedTime);
    view = glm::lookAt(position + waveOffset, position + waveOffset + camera->Front, camera->Up);
  } else {
    view = camera->GetViewMatrix();
  }

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

  auto& moveable = (*currentMoveable)->getMoveable();
  for (auto& [key, direction] : movementKeys) {
    if (glfwGetKey(window, key) == GLFW_PRESS) {
      moveable.ProcessKeyboard( direction, deltaTime);
    }
  }

  using Action = std::function<void()>;
  const std::vector<std::pair<int, Action>> keyBindings = {
    {GLFW_KEY_G, [this]() { loadUniforms(); }},
    {GLFW_KEY_R, [this]() { generateUniforms(); }},
    {GLFW_KEY_C, [this]() {
      (*currentMoveable)->deactivate();
      if (++currentMoveable == moveables.end())
        currentMoveable = moveables.begin();
      (*currentMoveable)->activate();
    }},
    {GLFW_KEY_N, [this]() { toggleNormalDisplay(); }},
    {GLFW_KEY_P, [this]() { toggleSimulation(); }},
    {GLFW_KEY_M, [this]() { toggleMesh(); }},
    {GLFW_KEY_J, [this]() { toggleDrawTriangles(); }},
    {GLFW_KEY_K, [this]() { toggleDrawLines(); }},
    {GLFW_KEY_L, [this]() { toggleWireframe(); }},
    {GLFW_KEY_B, [this]() { dumpUniforms(); }},
    {GLFW_KEY_O, [this]() { configuration->save(CONFIGURATION_DIR "/output.json"); }},
    {GLFW_KEY_T, [this]() { floatingCamera = !floatingCamera; }},
#ifndef __EMSCRIPTEN__
    {GLFW_KEY_F, [this, window]() { toggleFullscreen(window); }},
#endif
  };

  for (const auto& [key, action] : keyBindings) {
    executeIfPressed(window, key, action);
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

void OceanGestalt::onRender(const std::function<void()>& callback){
  renderThreadCallbacks.push_back(callback);
}

void OceanGestalt::pauseSimulation(bool){
  toggleSimulation();
} 
