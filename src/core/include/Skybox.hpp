#pragma once
#include "Drawable.hpp"

class Skybox : public Drawable {
 public:

  Skybox(std::shared_ptr<Configuration> context);
  void draw(Uniforms& uniforms) override;
  void drawNormals(Uniforms&, glm::mat4) override { /*do nothing*/ } ;
  void activate() override;
  
 private:
  GLuint vao;
  GLuint vbo;
  GLuint ebo;

  unsigned int indexCount;

  void drawMesh(Uniforms& uniforms, glm::mat4 transform) override;

  void bindVertices();
};
