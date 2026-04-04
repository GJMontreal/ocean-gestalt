#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include "GltfModel.hpp"

#include "Camera.hpp"
#include "Configuration.hpp"
#include "Light.hpp"
#include "Utilities.hpp"
#include "Wave.hpp"
#include "glError.hpp"

#include "stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <set>
#include <stdexcept>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static const float* accessorBase(const cgltf_accessor* acc) {
  const cgltf_buffer_view* bv = acc->buffer_view;
  const unsigned char* buf = static_cast<const unsigned char*>(bv->buffer->data);
  return reinterpret_cast<const float*>(buf + bv->offset + acc->offset);
}

static const unsigned int* accessorBaseU32(const cgltf_accessor* acc) {
  const cgltf_buffer_view* bv = acc->buffer_view;
  const unsigned char* buf = static_cast<const unsigned char*>(bv->buffer->data);
  return reinterpret_cast<const unsigned int*>(buf + bv->offset + acc->offset);
}

static const unsigned short* accessorBaseU16(const cgltf_accessor* acc) {
  const cgltf_buffer_view* bv = acc->buffer_view;
  const unsigned char* buf = static_cast<const unsigned char*>(bv->buffer->data);
  return reinterpret_cast<const unsigned short*>(buf + bv->offset + acc->offset);
}

// ---------------------------------------------------------------------------
// GltfModel
// ---------------------------------------------------------------------------

GltfModel::GltfModel(const std::string& glbPath, glm::vec3 origin,
                     std::shared_ptr<Configuration> context)
    : Model(context) {
  setOrigin(origin);
  load(glbPath);
}

GltfModel::~GltfModel() {
  for (auto& p : primitives) {
    if (p.VAO)     glDeleteVertexArrays(1, &p.VAO);
    if (p.VBO)     glDeleteBuffers(1, &p.VBO);
    if (p.EBO)     glDeleteBuffers(1, &p.EBO);
    if (p.lineEBO) glDeleteBuffers(1, &p.lineEBO);
  }
  for (auto& m : materials) {
    if (m.colorMap)  glDeleteTextures(1, &m.colorMap);
    if (m.normalMap) glDeleteTextures(1, &m.normalMap);
    if (m.mrMap)     glDeleteTextures(1, &m.mrMap);
  }
}

// ---------------------------------------------------------------------------
// Texture helpers
// ---------------------------------------------------------------------------

GLuint GltfModel::loadTexture(const unsigned char* data, int w, int h, int channels, bool srgb) {
  GLenum fmt    = (channels == 4) ? GL_RGBA : GL_RGB;
  GLenum intfmt = srgb
      ? ((channels == 4) ? GL_SRGB8_ALPHA8 : GL_SRGB8)
      : ((channels == 4) ? GL_RGBA8        : GL_RGB8);
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, intfmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  return tex;
}

GLuint GltfModel::fallbackTexture(unsigned char r, unsigned char g, unsigned char b) {
  unsigned char px[3] = {r, g, b};
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, px);
  glBindTexture(GL_TEXTURE_2D, 0);
  return tex;
}

// Load a texture referenced by a cgltf_texture_view; returns fallback if absent.
static GLuint loadCgltfTexture(const cgltf_texture_view* tv,
                                GLuint fallback,
                                std::function<GLuint(const unsigned char*, int, int, int)> upload) {
  if (!tv || !tv->texture || !tv->texture->image) return fallback;
  const cgltf_image* img = tv->texture->image;
  const cgltf_buffer_view* bv = img->buffer_view;
  if (!bv) return fallback;

  const unsigned char* buf =
      static_cast<const unsigned char*>(bv->buffer->data) + bv->offset;

  int w, h, channels;
  unsigned char* data = stbi_load_from_memory(buf, static_cast<int>(bv->size),
                                               &w, &h, &channels, 0);
  if (!data) {
    std::cout << "GltfModel: failed to decode image\n";
    return fallback;
  }
  GLuint tex = upload(data, w, h, channels);
  stbi_image_free(data);
  return tex;
}

// ---------------------------------------------------------------------------
// Loading
// ---------------------------------------------------------------------------

void GltfModel::load(const std::string& glbPath) {
  cgltf_options options{};
  cgltf_data* data = nullptr;

  if (cgltf_parse_file(&options, glbPath.c_str(), &data) != cgltf_result_success) {
    throw std::runtime_error("GltfModel: failed to parse " + glbPath);
  }
  if (cgltf_load_buffers(&options, data, glbPath.c_str()) != cgltf_result_success) {
    cgltf_free(data);
    throw std::runtime_error("GltfModel: failed to load buffers for " + glbPath);
  }

  // ---- Materials ----
  for (cgltf_size mi = 0; mi < data->materials_count; ++mi) {
    const cgltf_material& mat = data->materials[mi];
    GltfMaterial gm;

    // Color textures are sRGB-encoded; data textures (MR, normal) are linear.
    auto uploadSRGB   = [this](const unsigned char* d, int w, int h, int ch) {
      return loadTexture(d, w, h, ch, true);
    };
    auto uploadLinear = [this](const unsigned char* d, int w, int h, int ch) {
      return loadTexture(d, w, h, ch, false);
    };

    GLuint normalPixel = fallbackTexture(128, 128, 255);  // flat normal

    if (mat.has_pbr_metallic_roughness) {
      const auto& pbr = mat.pbr_metallic_roughness;

      // Color fallback: base_color_factor is linear per glTF spec — stored as GL_RGB8
      const float* c = pbr.base_color_factor;  // [R, G, B, A]
      GLuint colorFallback = fallbackTexture(
          static_cast<unsigned char>(c[0] * 255),
          static_cast<unsigned char>(c[1] * 255),
          static_cast<unsigned char>(c[2] * 255));
      gm.colorMap = loadCgltfTexture(&pbr.base_color_texture, colorFallback, uploadSRGB);

      // MR fallback: glTF packs roughness in G, metallic in B
      GLuint mrFallback = fallbackTexture(
          0,
          static_cast<unsigned char>(pbr.roughness_factor  * 255),
          static_cast<unsigned char>(pbr.metallic_factor   * 255));
      gm.mrMap = loadCgltfTexture(&pbr.metallic_roughness_texture, mrFallback, uploadLinear);
    } else {
      gm.colorMap = fallbackTexture(255, 255, 255);
      gm.mrMap    = fallbackTexture(0, 255, 0);  // full rough, no metal
    }
    gm.normalMap = loadCgltfTexture(&mat.normal_texture, normalPixel, uploadLinear);

    materials.push_back(gm);
  }

  // ---- Meshes / Primitives ----
  for (cgltf_size ni = 0; ni < data->nodes_count; ++ni) {
    const cgltf_node& node = data->nodes[ni];
    if (!node.mesh) continue;

    // Bake node world transform into vertex data so that scale/rotation/translation
    // from the DCC tool (e.g. Fusion 360 mm→m) are applied automatically.
    cgltf_float nm[16];
    cgltf_node_transform_world(&node, nm);
    glm::mat4 nodeTransform(
        glm::vec4(nm[0],  nm[1],  nm[2],  nm[3]),
        glm::vec4(nm[4],  nm[5],  nm[6],  nm[7]),
        glm::vec4(nm[8],  nm[9],  nm[10], nm[11]),
        glm::vec4(nm[12], nm[13], nm[14], nm[15]));
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(nodeTransform)));
    std::cout << "  node[" << ni << "] '" << (node.name ? node.name : "?") << "'"
              << " world scale ≈ ("
              << glm::length(glm::vec3(nodeTransform[0]))  << ", "
              << glm::length(glm::vec3(nodeTransform[1]))  << ", "
              << glm::length(glm::vec3(nodeTransform[2]))  << ")\n";

    for (cgltf_size pi = 0; pi < node.mesh->primitives_count; ++pi) {
      const cgltf_primitive& prim = node.mesh->primitives[pi];
      if (prim.type != cgltf_primitive_type_triangles) continue;

      std::vector<GltfVertex> verts;
      std::vector<unsigned int> idxs;

      // Find accessors by attribute name
      const cgltf_accessor* posAcc  = nullptr;
      const cgltf_accessor* uvAcc   = nullptr;
      const cgltf_accessor* normAcc = nullptr;
      const cgltf_accessor* tanAcc  = nullptr;

      for (cgltf_size ai = 0; ai < prim.attributes_count; ++ai) {
        const cgltf_attribute& attr = prim.attributes[ai];
        switch (attr.type) {
          case cgltf_attribute_type_position:  posAcc  = attr.data; break;
          case cgltf_attribute_type_texcoord:  if (attr.index == 0) uvAcc = attr.data; break;
          case cgltf_attribute_type_normal:    normAcc = attr.data; break;
          case cgltf_attribute_type_tangent:   tanAcc  = attr.data; break;
          default: break;
        }
      }

      if (!posAcc) continue;  // no positions — skip

      cgltf_size count = posAcc->count;
      verts.resize(count);

      // Stride-aware read via cgltf_accessor_read_float
      for (cgltf_size vi = 0; vi < count; ++vi) {
        float tmp[4];
        cgltf_accessor_read_float(posAcc, vi, tmp, 3);
        verts[vi].position = {tmp[0], tmp[1], tmp[2]};

        if (uvAcc) {
          cgltf_accessor_read_float(uvAcc, vi, tmp, 2);
          verts[vi].texCoord = {tmp[0], tmp[1]};
        }
        if (normAcc) {
          cgltf_accessor_read_float(normAcc, vi, tmp, 3);
          verts[vi].normal = {tmp[0], tmp[1], tmp[2]};
        } else {
          verts[vi].normal = {0.0f, 1.0f, 0.0f};
        }
        if (tanAcc) {
          cgltf_accessor_read_float(tanAcc, vi, tmp, 4);
          verts[vi].tangent = {tmp[0], tmp[1], tmp[2], tmp[3]};
        } else {
          verts[vi].tangent = {1.0f, 0.0f, 0.0f, 1.0f};
        }
      }

      // Apply node world transform to bake in scale/rotation/translation
      for (auto& v : verts) {
        v.position = glm::vec3(nodeTransform * glm::vec4(v.position, 1.0f));
        v.normal   = glm::normalize(normalMatrix * v.normal);
        glm::vec3 t3 = glm::normalize(normalMatrix * glm::vec3(v.tangent));
        v.tangent  = glm::vec4(t3, v.tangent.w);
      }

      // Indices
      if (prim.indices) {
        cgltf_size icount = prim.indices->count;
        idxs.resize(icount);
        for (cgltf_size ii = 0; ii < icount; ++ii) {
          idxs[ii] = static_cast<unsigned int>(cgltf_accessor_read_index(prim.indices, ii));
        }
      } else {
        idxs.resize(count);
        for (cgltf_size ii = 0; ii < count; ++ii) idxs[ii] = static_cast<unsigned int>(ii);
      }

      // Upload to GPU
      GltfPrimitive gp;
      gp.indexCount   = static_cast<int>(idxs.size());
      gp.materialIndex = prim.material
          ? static_cast<int>(prim.material - data->materials)
          : -1;

      glGenVertexArrays(1, &gp.VAO);
      glBindVertexArray(gp.VAO);

      glGenBuffers(1, &gp.VBO);
      glBindBuffer(GL_ARRAY_BUFFER, gp.VBO);
      glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GltfVertex),
                   verts.data(), GL_STATIC_DRAW);

      // we need to generate lines as well

      glGenBuffers(1, &gp.EBO);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gp.EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxs.size() * sizeof(unsigned int),
                   idxs.data(), GL_STATIC_DRAW);

      // location 0: position
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GltfVertex),
                            (void*)offsetof(GltfVertex, position));
      // location 1: texCoord
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GltfVertex),
                            (void*)offsetof(GltfVertex, texCoord));
      // location 2: normal
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GltfVertex),
                            (void*)offsetof(GltfVertex, normal));
      // location 3: tangent (vec4)
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(GltfVertex),
                            (void*)offsetof(GltfVertex, tangent));

      glBindVertexArray(0);

      // Line EBO — generated outside VAO context so VAO retains triangle EBO
      auto lineIdxs = generateWireframe(idxs);
      gp.lineIndexCount = static_cast<int>(lineIdxs.size());
      glGenBuffers(1, &gp.lineEBO);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gp.lineEBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   lineIdxs.size() * sizeof(GLuint), lineIdxs.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      primitives.push_back(gp);
    }
  }

  // ---- Diagnostics ----
  std::cout << "GltfModel: loaded '" << glbPath << "'\n";
  std::cout << "  primitives: " << primitives.size()
            << "  materials: "  << materials.size() << "\n";
  for (cgltf_size mi = 0; mi < data->materials_count; ++mi) {
    const cgltf_material& mat = data->materials[mi];
    auto colorLabel = [&]() -> const char* {
      if (!mat.has_pbr_metallic_roughness) return "fallback";
      if (mat.pbr_metallic_roughness.base_color_texture.texture) return "embedded";
      const float* c = mat.pbr_metallic_roughness.base_color_factor;
      return (c[0] < 0.99f || c[1] < 0.99f || c[2] < 0.99f) ? "factor" : "fallback";
    };
    auto mrLabel = [&]() -> const char* {
      if (!mat.has_pbr_metallic_roughness) return "fallback";
      if (mat.pbr_metallic_roughness.metallic_roughness_texture.texture) return "embedded";
      const auto& pbr = mat.pbr_metallic_roughness;
      return (pbr.roughness_factor < 0.99f || pbr.metallic_factor > 0.01f) ? "factor" : "fallback";
    };
    auto normalLabel = [&]() -> const char* {
      return mat.normal_texture.texture ? "embedded" : "fallback";
    };
    std::cout << "  mat[" << mi << "] "
              << (mat.name ? mat.name : "(unnamed)") << ":"
              << " color="              << colorLabel()
              << " normal="             << normalLabel()
              << " metallic-roughness=" << mrLabel()
              << "\n";
  }
  if (primitives.empty())
    std::cout << "GltfModel: warning — no primitives loaded\n";

  cgltf_free(data);
}

std::vector<GLuint> GltfModel::generateWireframe(std::vector<GLuint>& triangles) {
  std::set<std::pair<GLuint, GLuint>> edges;
  for (size_t i = 0; i + 2 < triangles.size(); i += 3) {
    GLuint a = triangles[i], b = triangles[i+1], c = triangles[i+2];
    edges.insert({std::min(a,b), std::max(a,b)});
    edges.insert({std::min(b,c), std::max(b,c)});
    edges.insert({std::min(a,c), std::max(a,c)});
  }
  std::vector<GLuint> lines;
  lines.reserve(edges.size() * 2);
  for (auto& e : edges) { lines.push_back(e.first); lines.push_back(e.second); }
  return lines;
}
// ---------------------------------------------------------------------------
// Draw
// ---------------------------------------------------------------------------

glm::mat4 GltfModel::getModelTransform(Transform transform) {
  auto m = glm::translate(glm::mat4(1.0f), getPosition() + transform.displacement);
  m = m * glm::mat4_cast(transform.rotation);
  auto r = getRotation();
  if (r.x != 0.0f) m = glm::rotate(m, glm::radians(r.x), glm::vec3(1,0,0));
  if (r.y != 0.0f) m = glm::rotate(m, glm::radians(r.y), glm::vec3(0,1,0));
  if (r.z != 0.0f) m = glm::rotate(m, glm::radians(r.z), glm::vec3(0,0,1));
  m = m * glm::scale(glm::mat4(1.0f), getScale());
  return m;
}

void GltfModel::drawMesh(Uniforms& uniforms, Transform transform) {
  auto shader = meshShader ? meshShader : configuration->getShader("model_gltf_pbr");
  auto _guard = ShaderScope(shader);

  shader->setUniform("model",              getModelTransform(transform));
  shader->setUniform("viewPos",            configuration->camera->getPosition());
  shader->setUniform("lightPos",           configuration->light->getPosition());
  shader->setUniform("time",               uniforms.time);
  shader->setUniform("isReflectionPass",   uniforms.isReflectionPass);

#ifdef __EMSCRIPTEN__
  shader->setUniform("projection", uniforms.projection);
  shader->setUniform("view",       uniforms.view);
#endif

  const auto& waves = configuration->getWaves();
  for (int i = 0; i < (int)waves.size(); i++) {
    shader->setUniform(string_format("waves[%d].amplitude",  i), waves[i]->amplitude);
    shader->setUniform(string_format("waves[%d].wavelength", i), waves[i]->wavelength);
    shader->setUniform(string_format("waves[%d].steepness",  i), waves[i]->steepness);
    shader->setUniform(string_format("waves[%d].phase",      i), waves[i]->phase);
    shader->setUniform(string_format("waves[%d].direction",  i), glm::vec3(waves[i]->direction, 0.0f));
  }

  for (auto& prim : primitives) {
    // Bind this primitive's material textures to fixed units
    const GltfMaterial& mat = (prim.materialIndex >= 0 && prim.materialIndex < (int)materials.size())
        ? materials[prim.materialIndex]
        : GltfMaterial{};

    // Use units 8-10 to avoid colliding with config-assigned units (envMap at 0).
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, mat.colorMap  ? mat.colorMap  : 0);
    shader->setUniform("colorMap", 8);

    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, mat.normalMap ? mat.normalMap : 0);
    shader->setUniform("normalMap", 9);

    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, mat.mrMap     ? mat.mrMap     : 0);
    shader->setUniform("bumpMap",      10);
    shader->setUniform("roughnessMap", 10);

    // Pass 1: shaded mesh above the waterline
    shader->setUniform("waterlineClip", 1.0f);
    glBindVertexArray(prim.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prim.EBO);
    glDrawElements(GL_TRIANGLES, prim.indexCount, GL_UNSIGNED_INT, nullptr);

    // Pass 2: wireframe below the waterline
    shader->setUniform("waterlineClip", -1.0f);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prim.lineEBO);
    glDrawElements(GL_LINES, prim.lineIndexCount, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
    shader->setUniform("waterlineClip", 0.0f);
  }

#ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif
}

void GltfModel::drawNormals(Uniforms& /*uniforms*/, Transform transform) {
#ifndef __EMSCRIPTEN__
  if (!normalShader) return;
  auto _guard = ShaderScope(normalShader);
  normalShader->setUniform("model", getModelTransform(transform));
  for (auto& prim : primitives) {
    glBindVertexArray(prim.VAO);
    glDrawElements(GL_LINES, prim.indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
  }
#endif
}

void GltfModel::drawWireframe(Uniforms& uniforms, Transform transform) {
  auto shader = configuration->getShader("gltf_wireframe");
  if (!shader) return;
  auto _guard = ShaderScope(shader);
  shader->setUniform("model",    getModelTransform(transform));
  shader->setUniform("lightPos", configuration->light->getPosition());
  shader->setUniform("viewPos",  configuration->camera->getPosition());
  shader->setUniform("time",     uniforms.time);
#ifdef __EMSCRIPTEN__
  shader->setUniform("projection", uniforms.projection);
  shader->setUniform("view",       uniforms.view);
#endif
  for (auto& prim : primitives) {
    glBindVertexArray(prim.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prim.lineEBO);
    glDrawElements(GL_LINES, prim.lineIndexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
  }
}
