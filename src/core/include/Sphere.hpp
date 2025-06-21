#pragma once

#include <glm/glm.hpp>
#include "Configuration.hpp"
#include "Drawable.hpp"

class Sphere : public Drawable {
 public:
  explicit Sphere(vec3 position);
  void draw(Uniforms& uniforms) override;
  void setShader(shared_ptr<ShaderProgram> shader);
  void setNormalShader(shared_ptr<ShaderProgram> shader);
 protected:
  void drawNormals(Uniforms& uniforms) override;
  void drawMesh(Uniforms& uniforms) override;

  void generateSphereMesh(std::vector<glm::vec3>& vertices,
                          std::vector<unsigned int>& indices,
                          unsigned int sectorCount = 36,
                          unsigned int stackCount = 18,
                          float radius = 1.0f);
  void bindVertices();

 private:
  bool shouldDrawMesh = true;
  bool shouldDrawNormals = true;
  
  std::shared_ptr<ShaderProgram> shader;
  std::shared_ptr<ShaderProgram> normalShader;
  
  glm::mat4 modelTransform;

  GLuint VAO;
  GLuint VBO;
  GLuint EBO;

  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<unsigned int> indices;
};
