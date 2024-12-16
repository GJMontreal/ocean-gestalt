/**
 * MyApplication.cpp
 * Contributors:
 *      * Arthur Sonzogni (author)
 * Licence:
 *      * MIT
 */
#include "MyApplication.hpp"

#include "Wave.hpp"
#include "Vertex.hpp"
#include "Ocean.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <vector>
#include "asset.hpp"
#include "glError.hpp"

MyApplication::MyApplication()
    : Application(),
      vertexShader(SHADER_DIR "/shader.vert", GL_VERTEX_SHADER),
      fragmentShader(SHADER_DIR "/shader.frag", GL_FRAGMENT_SHADER),
      shaderProgram({vertexShader, fragmentShader}),
      wireframeVertexShader(SHADER_DIR "/shader.vert", GL_VERTEX_SHADER),
      wireframeFragmentShader(SHADER_DIR "/wireframe.fs", GL_FRAGMENT_SHADER),
      wireframeShaderProgram({wireframeVertexShader,wireframeFragmentShader})
      // the order of our shader programs is important
      {
  // models.push_back(new Ocean(20,{wireframeShaderProgram,shaderProgram},this->getCamera()));
  models.push_back(new Ocean(40,{shaderProgram}, this->getCamera()));

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);

  glGenBuffers(1, &uboMatrices);
  glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
  glBufferData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), NULL, GL_STATIC_DRAW); 
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
  glCheckError(__FILE__, __LINE__);
}

void MyApplication::loop() {
  // exit on window close button pressed
  if (glfwWindowShouldClose(getWindow()))
    exit();

  // float t = fmod(getTime(),6.0f);

  projection = glm::perspective(glm::radians(getCamera()->Zoom),
                                getWindowRatio(), 0.1f, 100.f);

  Camera *camera = getCamera();
  view = camera->GetViewMatrix();

  // Set common shader uniforms
  glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
  glBufferSubData(GL_UNIFORM_BUFFER, 0,sizeof(glm::mat4),glm::value_ptr((projection)));
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4),sizeof(glm::mat4),glm::value_ptr((view)));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // clear
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for(int i=0; i < models.size(); i++){
    models[i]->draw();
  }
}
