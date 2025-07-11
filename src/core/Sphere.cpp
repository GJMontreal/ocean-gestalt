#include "Sphere.hpp"

#include "Camera.hpp"
#include "Light.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"
#include "glError.hpp"

#include <glm/gtc/matrix_transform.hpp>

Sphere::Sphere(glm::vec3 origin, glm::vec4 color, std::shared_ptr<Configuration> context, float radius):
Drawable(origin, context), color(color) {
  generateSphereMesh(vertices, indices, radius);
  bindVertices();
}

//Chat GPT generated!
void Sphere::generateSphereMesh(
    std::vector<glm::vec3>& vertices,
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
            vertices.push_back(pos);
            normals.push_back(glm::normalize(pos));
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

std::vector<VertexType> vertexData;
for (size_t i = 0; i < vertices.size(); ++i) {
    vertexData.push_back({vertices[i], normals[i],this->color});
}

// VAO
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);

// VBO
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(VertexType),
             vertexData.data(), GL_STATIC_DRAW);

// EBO
glGenBuffers(1, &EBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
             indices.data(), GL_STATIC_DRAW);

glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexType),
                      (void*)offsetof(VertexType, position));

glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexType),
                      (void*)offsetof(VertexType, normal));
glEnableVertexAttribArray(1);

glEnableVertexAttribArray(2);
glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexType),
                      (void*)offsetof(VertexType, color));

glBindVertexArray(0);
glCheckError(__FILE__, __LINE__);
}

void Sphere::draw(Uniforms& uniforms) {
  if(!shouldDraw){
    return;
  }
  
  auto transform = glm::mat4(1.f);
  if(preDraw){
    transform = preDraw(*this, uniforms.time);
  }

  if (shouldDrawMesh) {
    drawMesh(uniforms, transform);
  }

  #ifndef __EMSCRIPTEN__
  if (shouldDrawNormals){
    drawNormals(uniforms, transform);
  }
  #endif

  if(postDraw){
    postDraw(*this);
  }
}

void Sphere::drawMesh(Uniforms& uniforms, mat4 transform) {
  assert(shader); // ensure we have assigned a shader
  auto _guard = ShaderScope(shader);

  shader->setUniform("model", transform * this->getTransform());
  shader->setUniform("viewPos",
  this->getContext()->camera->getPosition());
  shader->setUniform("lightPos",this->getContext()->light->getPosition());

#ifdef __EMSCRIPTEN__
    shader->setUniform("projection", uniforms.projection);
    shader->setUniform("view", uniforms.view);
#endif

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

#ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif  
}

void Sphere::drawNormals(Uniforms& uniforms, mat4 transform) {
  auto _guard = ShaderScope(normalShader);
  
  shader->setUniform("model", this->getTransform() * transform);
  glBindVertexArray(VAO);
  glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

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