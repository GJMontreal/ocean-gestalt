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

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <iostream>
#include <vector>
#include "asset.hpp"
#include "glError.hpp"

// struct VertexType {
//   glm::vec3 position;
//   glm::vec3 normal;
//   glm::vec4 color;
// };

Wave awave(10.0f,10.0f,10.0f);
float heightMap(const glm::vec2 position) {
  return 2.0 * sin(position.x) * sin(position.y);
}

VertexType getHeightMap(const glm::vec2 position) {
  const glm::vec2 dx(1.0, 0.0);
  const glm::vec2 dy(0.0, 1.0);

  VertexType v;
  float h = heightMap(position);
  float hx = 100.f * (heightMap(position + 0.01f * dx) - h);
  float hy = 100.f * (heightMap(position + 0.01f * dy) - h);

  v.position = glm::vec3(position, h);
  v.normal = glm::normalize(glm::vec3(-hx, -hy, 1.0));

  float c = sin(h * 5.f) * 0.5 + 0.5;
  v.color = glm::vec4(c, 1.0 - c, 1.0, 1.0);
  return v;
}

MyApplication::MyApplication()
    : Application(),
      vertexShader(SHADER_DIR "/shader.vert", GL_VERTEX_SHADER),
      fragmentShader(SHADER_DIR "/shader.frag", GL_FRAGMENT_SHADER),
      shaderProgram({vertexShader, fragmentShader}),
      wireframeVertexShader(SHADER_DIR "/wireframe.vs", GL_VERTEX_SHADER),
      wireframeFragmentShader(SHADER_DIR "/wireframe.fs", GL_FRAGMENT_SHADER),
      wireframeGeometryShader(SHADER_DIR "/wireframe.gs", GL_GEOMETRY_SHADER),
      wireframeShaderProgram({wireframeVertexShader,wireframeFragmentShader,wireframeGeometryShader}),
      models{Model(10)} {
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

  // clear
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for(int i=0; i < models.size(); i++){
    models[i].draw(view,projection);
  }
}
