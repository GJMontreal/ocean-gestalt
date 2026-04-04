#pragma once

#include "Model.hpp"
#include <string>
#include <vector>

// Vertex layout for GLTF primitives — separate from the existing Vertex struct
// so Sphere/Ocean/etc. are unaffected.
struct GltfVertex {
  glm::vec3 position;  // location 0
  glm::vec2 texCoord;  // location 1
  glm::vec3 normal;    // location 2
  glm::vec4 tangent;   // location 3, W = handedness (-1 or +1)
};

struct GltfPrimitive {
  GLuint VAO = 0, VBO = 0, EBO = 0, lineEBO = 0;
  int indexCount = 0, lineIndexCount = 0;
  int materialIndex = -1;
};

struct GltfMaterial {
  GLuint colorMap        = 0;  // base color  (unit 0)
  GLuint normalMap       = 0;  // normal map  (unit 1)
  GLuint mrMap           = 0;  // metallic-roughness (unit 2)
};

class GltfModel : public Model {
public:
  GltfModel(const std::string& glbPath, glm::vec3 origin, std::shared_ptr<Configuration> context);
  ~GltfModel();

  void setOrigin(glm::vec3 origin) { Drawable::setOrigin(origin); }
  void setMeshShader(std::shared_ptr<ShaderProgram> shader) { meshShader = shader; }
  void setNormalShader(std::shared_ptr<ShaderProgram> shader) { normalShader = shader; }

protected:
  void drawMesh(Uniforms& uniforms, Transform transform) override;
  void drawNormals(Uniforms& uniforms, Transform transform) override;
  void drawWireframe(Uniforms& uniforms, Transform transform) override;

private:
  void load(const std::string& glbPath);
  GLuint loadTexture(const unsigned char* data, int w, int h, int channels);
  GLuint fallbackTexture(unsigned char r, unsigned char g, unsigned char b);

  glm::mat4 getModelTransform(Transform transform);

  std::vector<GLuint> generateWireframe(std::vector<GLuint>& triangles);

  std::vector<GltfPrimitive> primitives;
  std::vector<GltfMaterial>  materials;

  std::shared_ptr<ShaderProgram> meshShader;
  std::shared_ptr<ShaderProgram> normalShader;
};
