#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include "Mesh.hpp"
#include "Shader.hpp"

class Model {
 public:
  Model(int meshSize, std::vector<ShaderProgram> shaderPrograms);
  void draw();

  void setTransform(glm::mat4 transform);
 
 private:
  glm::mat4 transform = glm::mat4(1.0);

  std::vector<Mesh> meshes;
  std::vector<ShaderProgram> shaderPrograms;
};

#endif