#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
// #include "Configuration.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "asset.hpp"
#include "Camera.hpp"
#include "Uniforms.hpp"
#include "Drawable.hpp"

#include <memory>
class Configuration;
using std::shared_ptr;

class Model : public Drawable{
 public:
  explicit Model(shared_ptr<Configuration> configuration);

  virtual ~Model() = default;

  std::shared_ptr<Configuration> configuration; // TODO: this needs to be weak
  
  void draw(Uniforms& uniforms) override;
  
  void toggleRunning();

  bool isRunning()const;

  Mesh* getMesh(int index);

  protected:
  explicit Model(shared_ptr<Configuration> configuration, std::vector<Mesh> meshes);

  void drawNormals(Uniforms& uniforms, Transform transform) override;
  void drawMesh(Uniforms& uniforms, Transform transform) override;
  virtual void drawWireframe(Uniforms& uniforms);

  std::vector<Mesh> meshes;
  bool drawTriangles = true;

 private:
  glm::mat3 normalMatrix = glm::mat3(1.0);
  void calculateNormalMatrix(const glm::mat4& modelTransform, glm::mat3& aNormalMatrix);

  bool drawLines = false;
  bool running = true;

};

#endif