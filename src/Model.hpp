#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include "Mesh.hpp"
#include "Shader.hpp"
#include "asset.hpp"
#include "Camera.hpp"
#include "Uniforms.hpp"

#include <memory>

class Model {
 public:
  Model(int meshSize, const std::vector<ShaderProgram>& shaderPrograms, std::shared_ptr<Camera> camera);
  virtual ~Model() = default;

  void setTransform(const glm::mat4& transform);
  virtual void draw(Uniforms& uniforms);
  virtual void beginDrawing(ShaderProgram& program,  Uniforms& uniforms);
  void toggleDrawWireframe();
  void toggleDrawNormals();
  void toggleRunning();

  bool isRunning()const;
  bool shouldDrawNormals()const;
  bool shouldDrawWireframe()const;

  Mesh* getMesh(int index);
  ShaderProgram* getShaderProgram(int index);
  
  // virtual void setupWireframeShader();
  // virtual void setupMeshShader();

//These probably need to be pointers in order to work
  // ShaderProgram meshShader;
  // ShaderProgram wireframeShader;

// these could all be public, simplifying things
 private:
  glm::mat4 transform = glm::mat4(1.0);
  
  std::vector<ShaderProgram> shaderPrograms;
 
  std::shared_ptr<Camera> camera;
  std::vector<Mesh> meshes;
 
  bool drawWireframe = true;
  bool drawNormals = false;
  bool running = true;

#ifndef __EMSCRIPTEN__
  ShaderProgram wireframeShaderProgram =
      ShaderProgram({Shader(SHADER_DIR "/gerstner.vert", GL_VERTEX_SHADER),
                     Shader(SHADER_DIR "/simple.frag", GL_FRAGMENT_SHADER)});


  ShaderProgram normalShader =
      ShaderProgram({Shader(SHADER_DIR "/normal.vs", GL_VERTEX_SHADER),
                     Shader(SHADER_DIR "/normal.gs", GL_GEOMETRY_SHADER),
                     Shader(SHADER_DIR "/normal.fs", GL_FRAGMENT_SHADER)});
#else



#endif
};

#endif