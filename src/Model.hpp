#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include "Mesh.hpp"
#include "Shader.hpp"
#include "asset.hpp"
#include "camera.h"
#include "Uniforms.hpp"

class Model {
 public:
  Model(int meshSize, std::vector<ShaderProgram> shaderPrograms, Camera *camera);
  virtual ~Model() = default;

  void setTransform(glm::mat4 transform);
  virtual void draw(Uniforms uniforms);
  bool drawWireframe = true;
  bool drawNormals = true;

 private:
  glm::mat4 transform = glm::mat4(1.0);

 protected:
  Camera *camera;
  std::vector<Mesh> meshes;
  std::vector<ShaderProgram> shaderPrograms;
  
#ifndef __EMSCRIPTEN__
//are these being used
  ShaderProgram wireframeShaderProgram =
      ShaderProgram({Shader(SHADER_DIR "/shader.vert", GL_VERTEX_SHADER),
                     Shader(SHADER_DIR "/simple.frag", GL_FRAGMENT_SHADER)});


  ShaderProgram normalShader =
      ShaderProgram({Shader(SHADER_DIR "/normal.vs", GL_VERTEX_SHADER),
                     Shader(SHADER_DIR "/normal.gs", GL_GEOMETRY_SHADER),
                     Shader(SHADER_DIR "/normal.fs", GL_FRAGMENT_SHADER)});
#else



#endif
};

#endif