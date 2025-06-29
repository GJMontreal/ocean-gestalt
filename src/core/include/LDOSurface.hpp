#pragma once

#include "Drawable.hpp"

class ShaderProgram;

struct NDCVertex{
  glm::vec2 position;
};

// Coordinates here will be in screen space
// It's up to the shader to transform into world space
class LDOSurface : public Drawable {
 public:
  LDOSurface(glm::vec3 origin, std::shared_ptr<Configuration> context);
  void draw(Uniforms& uniforms) override;

  void setShader(std::shared_ptr<ShaderProgram> shader) {this->shader = shader;};
 protected:
  void drawNormals(Uniforms& uniforms, glm::mat4 transform) override;
  void drawMesh(Uniforms& uniforms, glm::mat4 transform) override;

 private:
  GLuint vao;
  GLuint vbo;
  GLuint ebo;
  std::vector<glm::vec2> vertices;
  std::vector<unsigned int> indices;

  std::shared_ptr<ShaderProgram> shader;

  void bindVertices();
  void generateMesh(std::vector<glm::vec2>& vertices,
                    std::vector<unsigned int>& indices, int gridSize);

};
