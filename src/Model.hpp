#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include "Mesh.hpp"
#include "Shader.hpp"
#include "asset.hpp"

class Model {
 public:
  Model(int meshSize, std::vector<ShaderProgram> shaderPrograms);
  virtual ~Model() = default;

  void setTransform(glm::mat4 transform);
  virtual void draw();

 private:
  glm::mat4 transform = glm::mat4(1.0);

 protected:
  std::vector<Mesh> meshes;
  std::vector<ShaderProgram> shaderPrograms;
  ShaderProgram wireframeShaderProgram =
      ShaderProgram({Shader(SHADER_DIR "/shader.vert", GL_VERTEX_SHADER),
                     Shader(SHADER_DIR "/wireframe.fs", GL_FRAGMENT_SHADER)});
};

#endif