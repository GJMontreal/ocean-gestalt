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

using std::shared_ptr;

class Model {
 public:
  explicit Model(shared_ptr<Configuration> configuration);

  virtual ~Model() = default;

  std::shared_ptr<Configuration> configuration;
  
  virtual void draw(Uniforms& uniforms);
  
  void toggleDrawWireframe();
  void toggleDrawMesh();
  void toggleDrawNormals();
  void toggleDrawTriangles();
  void toggleDrawLines();
  void toggleRunning();

  bool isRunning()const;

  Mesh* getMesh(int index);

  virtual void updateShaderUniforms() = 0;
  virtual void setWaveUniforms(const vector<shared_ptr<Wave>>& waves,
                         shared_ptr<ShaderProgram> program)const = 0;

// these could all be public, simplifying things
 private:
  
  glm::mat4 transform = glm::mat4(1.0);
  glm::mat3 normalMatrix = glm::mat3(1.0);
  
  void calculateNormalMatrix(const glm::mat4& modelTransform, glm::mat3& aNormalMatrix);
 
  std::vector<Mesh> meshes; //this is going to be only 1 mesh
 
  bool drawWireframe = true;
  bool drawNormals = false;
  
  bool drawMesh = false;  //it would be nice to specify these in our configuration
  bool drawTriangles = true;
  bool drawLines = true;

  bool running = true;

};

#endif