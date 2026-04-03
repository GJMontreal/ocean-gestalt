#include "ObjModel.hpp"

#include "Configuration.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Utilities.hpp"
#include "Wave.hpp"
#include "glError.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <stdexcept>

#include "stb_image.h"
#include <tiny_obj_loader.h>

static std::vector<Mesh> buildMeshes(const std::string& objPath) {
  auto slashPos = objPath.find_last_of("/\\");
  std::string directory = (slashPos != std::string::npos) ? objPath.substr(0, slashPos) : ".";

  tinyobj::ObjReaderConfig config;
  config.mtl_search_path = directory;

  tinyobj::ObjReader reader;
  if (!reader.ParseFromFile(objPath, config)) {
    throw std::runtime_error("ObjModel: failed to load " + objPath + ": " + reader.Error());
  }
  if (!reader.Warning().empty()) {
    std::cout << "ObjModel warning: " << reader.Warning() << std::endl;
  }

  const auto& attrib = reader.GetAttrib();
  const auto& shapes = reader.GetShapes();

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  for (const auto& shape : shapes) {
    for (const auto& idx : shape.mesh.indices) {
      Vertex v{};
      v.position = {
        attrib.vertices[3 * idx.vertex_index + 0],
        attrib.vertices[3 * idx.vertex_index + 1],
        attrib.vertices[3 * idx.vertex_index + 2],
      };
      if (idx.texcoord_index >= 0) {
        v.texCoord = {
          attrib.texcoords[2 * idx.texcoord_index + 0],
          1.0f - attrib.texcoords[2 * idx.texcoord_index + 1],
        };
      }
      // Store normal in the tangent slot (both are vec3 at location 2)
      if (idx.normal_index >= 0) {
        v.tangent = {
          attrib.normals[3 * idx.normal_index + 0],
          attrib.normals[3 * idx.normal_index + 1],
          attrib.normals[3 * idx.normal_index + 2],
        };
      }
      indices.push_back(static_cast<unsigned int>(vertices.size()));
      vertices.push_back(v);
    }
  }

  std::vector<Mesh> meshes;
  meshes.emplace_back(std::move(vertices), std::move(indices));
  return meshes;
}

static GLuint loadDiffuseTexture(const std::string& objPath) {
  auto slashPos = objPath.find_last_of("/\\");
  std::string directory = (slashPos != std::string::npos) ? objPath.substr(0, slashPos) : ".";

  tinyobj::ObjReaderConfig config;
  config.mtl_search_path = directory;
  tinyobj::ObjReader reader;
  reader.ParseFromFile(objPath, config);

  const auto& materials = reader.GetMaterials();
  if (!materials.empty() && !materials[0].diffuse_texname.empty()) {
    std::string texPath = directory + "/" + materials[0].diffuse_texname;
    int width, height, channels;
    unsigned char* data = stbi_load(texPath.c_str(), &width, &height, &channels, 3);
    if (data) {
      GLuint tex;
      glGenTextures(1, &tex);
      glBindTexture(GL_TEXTURE_2D, tex);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, 0);
      stbi_image_free(data);
      return tex;
    }
    std::cout << "ObjModel: could not load texture " << texPath << std::endl;
  }

  // 1x1 white fallback so the sampler is always bound to something valid
  unsigned char white[3] = {255, 255, 255};
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, white);
  glBindTexture(GL_TEXTURE_2D, 0);
  return tex;
}

ObjModel::ObjModel(const std::string& objPath, glm::vec3 origin, std::shared_ptr<Configuration> context)
    : Model(context, buildMeshes(objPath)) {
  setOrigin(origin);
  diffuseTexture = loadDiffuseTexture(objPath);
}

ObjModel::~ObjModel() {
  if (diffuseTexture) glDeleteTextures(1, &diffuseTexture);
}

void ObjModel::drawMesh(Uniforms& uniforms, Transform transform) {
  auto shader = meshShader ? meshShader : configuration->getShader("model_obj");
  auto _guard = ShaderScope(shader);

  shader->setUniform("model", getModelTransform(transform));
  shader->setUniform("viewPos", configuration->camera->getPosition());
  shader->setUniform("lightPos", configuration->light->getPosition());
  shader->setUniform("time", uniforms.time);
  shader->setUniform("isReflectionPass", uniforms.isReflectionPass);

#ifdef __EMSCRIPTEN__
  shader->setUniform("projection", uniforms.projection);
  shader->setUniform("view", uniforms.view);
#endif

  // Only bind the OBJ's diffuse texture when using the fallback shader.
  // Custom mesh shaders load their textures from shader.json configuration.
  if (!meshShader) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    shader->setUniform("colorMap", 0);
  }

  if (uniforms.shadowTexture) {
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, uniforms.shadowTexture);
    shader->setUniform("shadowMap", 4);
    shader->setUniform("lightSpaceMatrix", uniforms.lightSpaceMatrix);
  }

  const auto& waves = configuration->getWaves();
  for (int i = 0; i < (int)waves.size(); i++) {
    shader->setUniform(string_format("waves[%d].amplitude",  i), waves[i]->amplitude);
    shader->setUniform(string_format("waves[%d].wavelength", i), waves[i]->wavelength);
    shader->setUniform(string_format("waves[%d].steepness",  i), waves[i]->steepness);
    shader->setUniform(string_format("waves[%d].phase",      i), waves[i]->phase);
    shader->setUniform(string_format("waves[%d].direction",  i), glm::vec3(waves[i]->direction, 0.0f));
  }

  // Pass 1: shaded mesh above the waterline
  shader->setUniform("waterlineClip", 1.0f);
  for (auto& mesh : meshes) mesh.draw();

  // Pass 2: wireframe below the waterline
  shader->setUniform("waterlineClip", -1.0f);
  shader->setUniform("wireframeColor", glm::vec3(0.4f, 0.5f, 0.55f));
  for (auto& mesh : meshes) mesh.drawWireframe();

  shader->setUniform("waterlineClip", 0.0f);

#ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif
}

void ObjModel::drawNormals(Uniforms& /*uniforms*/, Transform transform) {
#ifndef __EMSCRIPTEN__
  if (!normalShader) return;
  auto _guard = ShaderScope(normalShader);
  normalShader->setUniform("model", getModelTransform(transform));
  for (auto& mesh : meshes) {
    mesh.drawAsLines();
  }
#endif
}

void ObjModel::drawWireframe(Uniforms& uniforms) {
  auto shader = configuration->getShader("model_obj_wireframe");
  auto _guard = ShaderScope(shader);
  shader->setUniform("model", getModelTransform(Transform{}));
  shader->setUniform("lightPos", configuration->light->getPosition());
  shader->setUniform("viewPos", configuration->camera->getPosition());
#ifdef __EMSCRIPTEN__
  shader->setUniform("projection", uniforms.projection);
  shader->setUniform("view", uniforms.view);
#endif
  for (auto& mesh : meshes) {
    mesh.drawWireframe();
  }
}

glm::mat4 ObjModel::getModelTransform(Transform transform) {
  auto m = glm::translate(glm::mat4(1.0f), getPosition() + transform.displacement);
  m = m * glm::mat4_cast(transform.rotation);
  m = m * glm::scale(glm::mat4(1.0f), getScale());
  return m;
}
