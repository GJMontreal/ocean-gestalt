#pragma once

#include <glm/glm.hpp>
#include "Configuration.hpp"
#include "Drawable.hpp"

class Sphere : public Drawable {
 public:
  explicit Sphere(vec3 position, vec4 color, std::shared_ptr<Configuration> context, float radius = 1.f);
  void draw(Uniforms& uniforms) override;
  void setShader(std::shared_ptr<ShaderProgram> shader);
  void setNormalShader(std::shared_ptr<ShaderProgram> shader);
  
  void enableDrawing(bool should) {
    shouldDraw = should;
  };

 protected:
  void drawNormals(Uniforms& uniforms, glm::mat4 transform) override;
  void drawMesh(Uniforms& uniforms, glm::mat4 transform) override;

  void generateSphereMesh(std::vector<glm::vec3>& vertices,
                          std::vector<unsigned int>& indices,
                          float radius = 1.0f,
                          unsigned int sectorCount = 36,
                          unsigned int stackCount = 18
                          );
  void bindVertices();

 private:
  
  bool shouldDraw = true;
  bool shouldDrawMesh = true;
  bool shouldDrawNormals = false;

  std::shared_ptr<ShaderProgram> shader;
  std::shared_ptr<ShaderProgram> normalShader;
  
  glm::mat4 modelTransform;

  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  
  glm::vec4 color;
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<unsigned int> indices;
};
