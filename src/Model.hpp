#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include "Configuration.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "asset.hpp"
#include "Camera.hpp"
#include "Uniforms.hpp"

#include <memory>

class Model {
 public:
  Model(int meshSize, const std::vector<ShaderProgram>& shaderPrograms, std::shared_ptr<Camera> camera);
  Model(int meshSize, std::shared_ptr<Configuration> configuration);

  virtual ~Model() = default;

  virtual void draw(Uniforms& uniforms);
  virtual void beginDrawing(std::shared_ptr<ShaderProgram> program,  Uniforms& uniforms);
  
  void toggleDrawWireframe();
  void toggleDrawMesh();
  void toggleDrawNormals();
  void toggleRunning();

  bool isRunning()const;
  bool shouldDrawNormals()const;
  bool shouldDrawWireframe()const;

  Mesh* getMesh(int index);

// these could all be public, simplifying things
 private:
  std::shared_ptr<Configuration> configuration;
  glm::mat4 transform = glm::mat4(1.0);
  glm::mat3 normalMatrix = glm::mat3(1.0);
  
  void calculateNormalMatrix(const glm::mat4& modelTransform, glm::mat3& aNormalMatrix);
 
  std::vector<Mesh> meshes; //this is going to be only 1 mesh
 
  bool drawWireframe = true;
  bool drawNormals = false;
  
  bool drawMesh = true;
  bool drawTriangles = true;
  bool drawLines = true;

  bool running = true;
};

#endif