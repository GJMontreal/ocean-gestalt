#pragma once

#include "Drawable.hpp"

class ShaderProgram;

struct NDCVertex{
  glm::vec2 position;
};

// Coordinates here will be in screen space
// It's up to the shader to transform into world space
class LODSurface : public Drawable {
 public:
  LODSurface(glm::vec3 origin, std::shared_ptr<Configuration> context);
  void draw(Uniforms& uniforms) override;

  void setShader(std::shared_ptr<ShaderProgram> shader) {this->shader = shader;};
  void setProjectionShader(std::shared_ptr<ShaderProgram> shader) { this->projectionShader = shader;};
  void setRayShader(std::shared_ptr<ShaderProgram> shader) {this->rayShader = shader;};

 protected:
  void drawNormals(Uniforms& uniforms, glm::mat4 transform) override;
  void drawMesh(Uniforms& uniforms, glm::mat4 transform) override;

 private:
  GLuint vao;
  GLuint vbo;
  GLuint ebo;
  std::vector<glm::vec2> vertices;
  std::vector<unsigned int> indices;

  std::shared_ptr<ShaderProgram> shader; //couldn't this be in the base class
  std::shared_ptr<ShaderProgram> projectionShader;
  std::shared_ptr<ShaderProgram> rayShader;

  GLuint rayVAO;
  GLuint rayVBO;
  GLuint rayEBO;

  std::vector<unsigned int> rayIndices;

  void setCommonUniforms(std::shared_ptr<ShaderProgram> shader, Uniforms& uniforms);
  
  void bindVertices();
  void generateMesh(std::vector<glm::vec2>& vertices,
                    std::vector<unsigned int>& indices, int width, int height);

  void bindRays();
  void generateRays();

  void drawRays(Uniforms& uniforms);
  void drawProjectedMesh(Uniforms& uniforms);

  void debugRay(Uniforms& uniforms);
};
