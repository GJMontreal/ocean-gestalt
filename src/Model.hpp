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
  Model(int meshSize, const std::vector<ShaderProgram>& shaderPrograms, Camera *camera);
  virtual ~Model() = default;

  void setTransform(const glm::mat4& transform);
  virtual void draw(const Uniforms& uniforms);
  
  void toggleDrawWireframe();
  void toggleDrawNormals();
  void toggleRunning();

  bool isRunning()const;
  bool shouldDrawNormals()const;
  bool shouldDrawWireframe()const;

  Mesh* getMesh(int index);

 private:
  glm::mat4 transform = glm::mat4(1.0);
  
  Camera *camera;
  std::vector<Mesh> meshes;
  std::vector<ShaderProgram> shaderPrograms;
  
  bool drawWireframe = true;
  bool drawNormals = true;
  bool running = true;

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