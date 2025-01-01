#include "MyApplication.hpp"

#include "Configuration.hpp"
#include "Ocean.hpp"
#include "Uniforms.hpp"
#include "asset.hpp"
#include "glError.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <vector>

#include <nlohmann/json.hpp>

MyApplication::MyApplication()
    : Application()
{
  auto config = std::make_shared<Configuration>(CONFIGURATION_DIR "/config.json");
  this->camera = config->camera;
  models.push_back(new Ocean(20,config));
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
  // exit on window close button pressed
  if (glfwWindowShouldClose(getWindow()))
    exit();

  projection = glm::perspective(glm::radians(getCamera()->Zoom),
                                getWindowRatio(), 0.1f, 100.f);

  auto camera = getCamera();
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
