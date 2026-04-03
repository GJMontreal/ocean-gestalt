#pragma once

#include "Model.hpp"
#include <string>

class ObjModel : public Model {
public:
  ObjModel(const std::string& objPath, glm::vec3 origin, std::shared_ptr<Configuration> context);
  ~ObjModel();

  void setOrigin(glm::vec3 origin) { Drawable::setOrigin(origin); }
  void setNormalShader(std::shared_ptr<ShaderProgram> shader) { normalShader = shader; }

protected:
  void drawMesh(Uniforms& uniforms, Transform transform) override;
  void drawNormals(Uniforms& uniforms, Transform transform) override;
  void drawWireframe(Uniforms& uniforms) override;

private:
  static std::vector<Mesh> loadMeshes(const std::string& objPath);
  GLuint diffuseTexture = 0;
  std::shared_ptr<ShaderProgram> normalShader;

  glm::mat4 getModelTransform(Transform transform);
};
