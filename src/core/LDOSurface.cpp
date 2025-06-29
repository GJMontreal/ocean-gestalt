#include "LDOSurface.hpp"

#include "Camera.hpp"
#include "Configuration.hpp"
#include "glError.hpp"
#include "Shader.hpp"
#include <glm/gtx/string_cast.hpp>


LDOSurface::LDOSurface(glm::vec3 origin, std::shared_ptr<Configuration> context)
    : Drawable(origin, context) {
      generateMesh(vertices, indices, 128);
      bindVertices();
    }

void LDOSurface::draw(Uniforms& uniforms) {
   auto transform = glm::mat4(1.f);
   drawMesh(uniforms, transform);
}

void LDOSurface::drawNormals(Uniforms& uniforms, glm::mat4 transform) {}

void LDOSurface::drawMesh(Uniforms& uniforms, glm::mat4 transform) {
  assert(shader); // ensure we have assigned a shader
  auto _guard = ShaderScope(shader);
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
glCheckError(__FILE__, __LINE__);
// shader->setUniform("viewPos", this->getContext()->camera->getPosition());

glm::mat4 invViewProjection = glm::inverse(uniforms.projection * uniforms.view);

shader->setUniform("invViewProjection", invViewProjection);

// std::cout << "projection:\n" << glm::to_string(uniforms.projection) << std::endl;
// std::cout << "view:\n" << glm::to_string(uniforms.view) << std::endl;
// std::cout << "invViewProj:\n" << glm::to_string(invViewProjection) << std::endl;

glCheckError(__FILE__, __LINE__);
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
glCheckError(__FILE__, __LINE__);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif  
}

void LDOSurface::generateMesh(std::vector<glm::vec2>& vertices,
                              std::vector<unsigned int>& indices,
                              int gridSize) {
  vertices.clear();
  indices.clear();

  // Step size from -1 to 1
  float step = 2.0f / (gridSize - 1);

  for (int y = 0; y < gridSize; ++y) {
    for (int x = 0; x < gridSize; ++x) {
      float px = -1.0f + x * step;
      float py = -1.0f + y * step;
      vertices.push_back({glm::vec2(px, py)});
    }
  }

  // Generate indices (triangle grid)
  for (int y = 0; y < gridSize - 1; ++y) {
    for (int x = 0; x < gridSize - 1; ++x) {
      int i = y * gridSize + x;

      indices.push_back(i);
      indices.push_back(i + gridSize);
      indices.push_back(i + 1);

      indices.push_back(i + 1);
      indices.push_back(i + gridSize);
      indices.push_back(i + gridSize + 1);
    }
  }
}

void LDOSurface::bindVertices() {
  std::vector<NDCVertex> vertexData;
  vertexData.reserve(vertices.size());
  for (auto vertex : vertices) {
    vertexData.push_back({vertex});
  }

  // VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // VBO
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(NDCVertex),
               vertexData.data(), GL_STATIC_DRAW);

  // EBO
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(NDCVertex),
                        (void*)offsetof(NDCVertex, position));

  glBindVertexArray(0);
  glCheckError(__FILE__, __LINE__);
}