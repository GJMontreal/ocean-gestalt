#include "Mesh.hpp"

#include "asset.hpp"
#include "glError.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>

Mesh::Mesh(int meshSize): 
// all these should be customizable
// get it working first
  color(1.0f,1.0,0.0f,1.0f),
  vertexShader(SHADER_DIR "/shader.vert", GL_VERTEX_SHADER),
  fragmentShader(SHADER_DIR "/shader.frag", GL_FRAGMENT_SHADER),
  shaderProgram({vertexShader, fragmentShader})
{
  size = meshSize;

  // I'd rather there was an explicit call to set the model transform
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  // model = glm::scale(model,{.10,.10,.10});  // we'll figure out later how to scale our model
  // our shader seems to be taking the scaling into account incorrectly
  generateMesh(size);
}

Mesh::~Mesh(){

}


// why is this messing up and drawing the previous set of vertices?
// maybe a shader shouldn't be attached to a mesh?
void Mesh::draw(glm::mat4 view, glm::mat4 projection){
  shaderProgram.use();

  // send uniforms
  shaderProgram.setUniform("projection", projection);
  shaderProgram.setUniform("view", view);
  shaderProgram.setUniform("model", model);

  glCheckError(__FILE__, __LINE__);

  glBindVertexArray(vao);
// not sure we need to do this
  // glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  shaderProgram.setAttribute("position", 3, sizeof(VertexType),
                             offsetof(VertexType, position));
  shaderProgram.setAttribute("normal", 3, sizeof(VertexType),
                             offsetof(VertexType, normal));
  shaderProgram.setAttribute("color", 4, sizeof(VertexType),
                             offsetof(VertexType, color));

  glCheckError(__FILE__, __LINE__);
  glDrawElements(GL_TRIANGLES,         // mode
                 size * size * 2 * 3,  // count
                 GL_UNSIGNED_INT,      // type
                 NULL                  // element array buffer offset
  );
  shaderProgram.unuse();
  glBindVertexArray(0);
}

// The mesh will be symmetrical in x and y
void Mesh::generateMesh(int size){
  // Mesh will have vertex and normal for the moment
  std::vector<VertexType> vertices;
  std::vector<GLuint> index;

  for (int y = 0; y <= size; ++y)
    for (int x = 0; x <= size; ++x) {
      float xx = (x - size / 2 );
      float yy = (y - size / 2 );
      vertices.push_back(generateVertex({xx, yy},color));
    }

  for (int y = 0; y < size; ++y)
    for (int x = 0; x < size; ++x) {
      index.push_back((x + 0) + (size + 1) * (y + 0));
      index.push_back((x + 1) + (size + 1) * (y + 0));
      index.push_back((x + 1) + (size + 1) * (y + 1));

      index.push_back((x + 1) + (size + 1) * (y + 1));
      index.push_back((x + 0) + (size + 1) * (y + 1));
      index.push_back((x + 0) + (size + 1) * (y + 0));
    }
  // creation of the vertex array buffer----------------------------------------
  std::cout << "vertices=" << vertices.size() << std::endl;
  std::cout << "index=" << index.size() << std::endl;

  // vbo
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexType),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // ibo
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLuint),
               index.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // vao
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // bind vbo
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
 
  // bind the ibo
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  // vao end
  glBindVertexArray(0);
}

VertexType Mesh::generateVertex(const glm::vec2 position, glm::vec4 color) {
  const glm::vec2 dx(1.0, 0.0);
  const glm::vec2 dy(0.0, 1.0);

  VertexType v;
  float h = 0;
  v.position = glm::vec3(position, h);
  v.normal = glm::normalize(glm::vec3(-h, h, 1.0));

  v.color = color;
  return v;
}