#include "Sphere.hpp"

#include "glError.hpp"

#include <glm/gtc/matrix_transform.hpp>

Sphere::Sphere(vec3 position,std::shared_ptr<Configuration> context):Drawable(context) {
  this->setOrigin(position);
  generateSphereMesh(vertices, indices);
  bindVertices();
}

//Chat GPT generated!
void Sphere::generateSphereMesh(
    std::vector<glm::vec3>& vertices,
    std::vector<unsigned int>& indices,
    unsigned int sectorCount,
    unsigned int stackCount,
    float radius)
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
    vertexData.push_back({vertices[i], normals[i],glm::vec4(0.0f,1.0f,1.0f,1.0f)});
}

// VAO
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);

// VBO
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(VertexType), vertexData.data(), GL_STATIC_DRAW);

// EBO
glGenBuffers(1, &EBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexType), (void*)offsetof(VertexType, position));

glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexType), (void*)offsetof(VertexType, normal));
glEnableVertexAttribArray(1);

glEnableVertexAttribArray(2);
glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,sizeof(VertexType), (void*)offsetof(VertexType,color));
  

glBindVertexArray(0);
  glCheckError(__FILE__, __LINE__);
}

void Sphere::draw(Uniforms& uniforms) {
  if (shouldDrawMesh) {
    drawMesh(uniforms);
  }
  #ifndef __EMSCRIPTEN__
  if (shouldDrawNormals){
    drawNormals(uniforms);
  }
  #endif
}

void Sphere::drawMesh(Uniforms& uniforms) {
  shader->activate();

  shader->setUniform("model", this->getTransform());
  shader->setUniform("viewPos",
  this->getContext()->camera->position);
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

  shader->deactivate();
  glCheckError(__FILE__, __LINE__);
}

void Sphere::drawNormals(Uniforms& uniforms) {
  normalShader->activate();
  
  shader->setUniform("model", this->getTransform());
  glBindVertexArray(VAO);
  glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

  normalShader->deactivate();
  glCheckError(__FILE__, __LINE__);
}

void Sphere::setShader(shared_ptr<ShaderProgram> shader) {
  this->shader = shader;
}

void Sphere::setNormalShader(shared_ptr<ShaderProgram> shader) {
  this->normalShader = shader;
}