#include "Skybox.hpp"

#include "Configuration.hpp"
#include "glError.hpp"
#include "Shader.hpp"

#include <iostream>

Skybox::Skybox(std::shared_ptr<Configuration> context):Drawable(glm::vec3(0.f,0.f,0.f),context){
  bindVertices();
}

void Skybox::bindVertices() {
  std::vector<glm::vec3> vertices = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1},
                                     {-1, 1, -1},  {-1, -1, 1}, {1, -1, 1},
                                     {1, 1, 1},    {-1, 1, 1}};

  std::vector<unsigned int> indices = {// -Z (back)
                                       2, 1, 0, 0, 3, 2,
                                       // +Z (front)
                                       4, 5, 6, 6, 7, 4,
                                       // -X (left)
                                       0, 4, 7, 7, 3, 0,
                                       // +X (right)
                                       1, 2, 6, 6, 5, 1,
                                       // +Y (top)
                                       3, 7, 6, 6, 2, 3,
                                       // -Y (bottom)
                                       0, 1, 5, 5, 4, 0};

  indexCount = indices.size();

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
               vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        (void*)nullptr);

  glBindVertexArray(0);
  glCheckError(__FILE__, __LINE__);
}

void Skybox::draw(Uniforms& uniforms){
  if(getIfShouldDrawMesh()){
    drawMesh(uniforms, Transform());
  }
}

void Skybox::drawMesh(Uniforms& uniforms, Transform transform) {
  auto shader = getContext()->getShader("skybox");
  auto _guard = ShaderScope(shader);

  glFrontFace(GL_CW);
  glDepthMask(GL_FALSE); 
  // glDepthFunc(GL_LEQUAL);
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
  // glDepthFunc(GL_LESS);
  glFrontFace(GL_CCW);
  glDepthMask(GL_TRUE);
#ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif
}

void Skybox::activate(){
  std::cout << "Skybox selected" << std::endl;
}
