#include "Sphere.hpp"

#include "Camera.hpp"
#include "Light.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"
#include "glError.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "Utilities.hpp"
#include "Wave.hpp"

#include <set>
#include <utility>

Sphere::Sphere(glm::vec3 origin, glm::vec4 color, std::shared_ptr<Configuration> context, float radius):
Drawable(origin, context), color(color) {
  generateSphereMesh(vertices, indices, 1.0);
  generateEdgeIndices();
  this->setScale(glm::vec3(radius));
  bindVertices();
}

void Sphere::generateEdgeIndices() {
  std::set<std::pair<unsigned int, unsigned int>> edgeSet;
  for (size_t i = 0; i + 2 < indices.size(); i += 3) {
    unsigned int i0 = indices[i], i1 = indices[i+1], i2 = indices[i+2];
    edgeSet.insert({std::min(i0,i1), std::max(i0,i1)});
    edgeSet.insert({std::min(i1,i2), std::max(i1,i2)});
    edgeSet.insert({std::min(i0,i2), std::max(i0,i2)});
  }
  for (auto& e : edgeSet) {
    edgeIndices.push_back(e.first);
    edgeIndices.push_back(e.second);
  }
}

//Chat GPT generated!
void Sphere::generateSphereMesh(
    std::vector<Vertex>& vertices,
    std::vector<unsigned int>& indices,
    float radius,
    unsigned int sectorCount,
    unsigned int stackCount
   )
{
    for (unsigned int i = 0; i <= stackCount; ++i) {
        float stackAngle = M_PI / 2 - i * M_PI / stackCount; // from +π/2 to -π/2
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (unsigned int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * 2 * M_PI / sectorCount;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            glm::vec3 pos(x,y,z);
            
            // Texture coordinates from spherical projection
            float u = (float)j / sectorCount;
            float v = (float)i / stackCount;
            glm::vec2 texCoord(u, 1.0f - v);  // flip V for OpenGL convention
            
            // Tangent vector (direction of increasing longitude)
            glm::vec3 tangent = glm::normalize(glm::vec3(-y, x, 0.0f));  // ∂/∂longitude
            if (std::abs(z / radius) > 0.999f)  // handle poles
                tangent = glm::vec3(1.0f, 0.0f, 0.0f);

            vertices.push_back({pos, texCoord, tangent});
        }
    }

    for (unsigned int i = 0; i < stackCount; ++i) {
        for (unsigned int j = 0; j < sectorCount; ++j) {
            unsigned int first = i * (sectorCount + 1) + j;
            unsigned int second = first + sectorCount + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}

void Sphere::bindVertices(){
// VAO
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);

// VBO
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
             vertices.data(), GL_STATIC_DRAW);

// EBO
glGenBuffers(1, &EBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
             indices.data(), GL_STATIC_DRAW);

glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                      (void*)offsetof(Vertex, position));

glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                      (void*)offsetof(Vertex, texCoord));
glEnableVertexAttribArray(1);

glEnableVertexAttribArray(2);
glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                      (void*)offsetof(Vertex, tangent));

glBindVertexArray(0);

// Edge VAO — same VBO, separate EBO for GL_LINES wireframe pass
glGenVertexArrays(1, &edgeVAO);
glBindVertexArray(edgeVAO);

glBindBuffer(GL_ARRAY_BUFFER, VBO);

glGenBuffers(1, &edgeEBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeEBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, edgeIndices.size() * sizeof(unsigned int),
             edgeIndices.data(), GL_STATIC_DRAW);

glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                      (void*)offsetof(Vertex, position));
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                      (void*)offsetof(Vertex, texCoord));
glEnableVertexAttribArray(1);
glEnableVertexAttribArray(2);
glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                      (void*)offsetof(Vertex, tangent));

glBindVertexArray(0);
glCheckError(__FILE__, __LINE__);
}

void Sphere::draw(Uniforms& uniforms) {
  Transform transform;
  
  if(preDraw){
    transform = preDraw(*this, uniforms.time);
  }

  if (getIfShouldDrawMesh()) {
    drawMesh(uniforms, transform);
  }

  #ifndef __EMSCRIPTEN__
  if (getIfShouldDrawNormals()){
    drawNormals(uniforms, transform);
  }
  #endif

}

void Sphere::drawMesh(Uniforms& uniforms, Transform transform) {
  assert(shader); // ensure we have assigned a shader
  auto _guard = ShaderScope(shader);
 
  shader->setUniform("model",   getModelTransform(transform));
  shader->setUniform("viewPos", this->getContext()->camera->getPosition());
  shader->setUniform("lightPos",this->getContext()->light->getPosition());
  shader->setUniform("time", uniforms.time);
  shader->setUniform("isReflectionPass", uniforms.isReflectionPass);

#ifdef __EMSCRIPTEN__
    shader->setUniform("projection", uniforms.projection);
    shader->setUniform("view", uniforms.view);
#endif

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

#ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif
}

void Sphere::drawNormals(Uniforms& uniforms, Transform transform) {
  auto _guard = ShaderScope(normalShader);
  normalShader->setUniform("model", getModelTransform(transform));
  glBindVertexArray(VAO);
  glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
#ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif
}

void Sphere::setShader(shared_ptr<ShaderProgram> shader) {
  this->shader = shader;
}

void Sphere::setNormalShader(shared_ptr<ShaderProgram> shader) {
  this->normalShader = shader;
}

glm::mat4 Sphere::getModelTransform(Transform transform) {
    // TODO: why are the moveable and drawables position different?
  auto model = glm::translate(glm::mat4(1.0f),
                              this->getPosition() + transform.displacement);

  model = model * glm::mat4_cast(transform.rotation);
  model = model * glm::scale(glm::mat4(1.0f), this->getScale());
  return model;
}