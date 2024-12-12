#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include "Mesh.hpp"
#include "Shader.hpp"

class Model {
 public:
  Model(int meshSize);
  void draw(glm::mat4 view, glm::mat4 projection);

  void setTransform(glm::mat4 transform);
 
 private:
  std::vector<Mesh> meshes;
  std::vector<Shader> shaders;

  glm::mat4 transform = glm::mat4(1.0);
};

#endif