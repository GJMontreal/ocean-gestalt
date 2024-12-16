#include "Mesh.hpp"

#include "asset.hpp"
#include "glError.hpp"
#include "glm/fwd.hpp"

// #include <cmath>
#include <iostream>
#include <vector>

#include <glm/gtx/normal.hpp>

Mesh::Mesh(int meshSize): 
// all these should be customizable
// get it working first
  color(1.0f,1.0,0.0f,1.0f)
{
  size = meshSize;
  generateMesh(size);
}

Mesh::~Mesh() {}

int Mesh::getSize(){
  return size;
}

GLuint Mesh::getVbo(){
  return vbo;
}

std::vector<GLuint> Mesh::getTriangularIndices(){
  return triangularMeshIndices;
}

void Mesh::drawWireframe() {
  glBindVertexArray(wireframeVao);

  glDrawElements(GL_LINES,         // mode
                 size * size * 6,  // count // how did we calculate this
                 GL_UNSIGNED_INT,  // type
                 NULL              // element array buffer offset
  );

  glCheckError(__FILE__, __LINE__);

  glBindVertexArray(0);
}

void Mesh::drawNormals(){

}

void Mesh::draw() {
    glBindVertexArray(vao);

    glDrawElements(GL_TRIANGLES,         // mode
                   size * size * 2 * 3,  // count
                   GL_UNSIGNED_INT,      // type
                   NULL                  // element array buffer offset
    );

    glCheckError(__FILE__, __LINE__);

    glBindVertexArray(0);
}

// The mesh will be symmetrical in x and y
void Mesh::generateMesh(int size) {
  // Mesh will have vertex and normal for the moment
  std::vector<VertexType> vertices;
  // std::vector<GLuint> indices;

  for (int y = 0; y <= size; ++y)
    for (int x = 0; x <= size; ++x) {
      float xx = (x - size / 2);
      float yy = (y - size / 2);
      vertices.push_back(generateVertex({xx, yy}, color));
    }

  // generate indices for a triangular mesh
  triangularMeshIndices = generateTriangularIndices(size);
  // creation of the vertex array buffer----------------------------------------
  std::cout << "vertices=" << vertices.size() << std::endl;
  std::cout << "index=" << triangularMeshIndices.size() << std::endl;

  // given a vector of vertices and a vector of indices - generate normals
  // calculateNormals(vertices, triangularMeshIndices);
  // vbo
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexType),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // ibo
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangularMeshIndices.size() * sizeof(GLuint),
               triangularMeshIndices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // vao
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // bind vbo
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // bind the ibo
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  // normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // color attribute
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  
  // vao end
  glBindVertexArray(0);
  glCheckError(__FILE__, __LINE__);

  std::vector<GLuint> wireframeIndices = generateWireframeIndices(size);
  
  //repeat to generate buffers for wireframe
  glGenBuffers(1, &wireframeIbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireframeIbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, wireframeIndices.size() * sizeof(GLuint),
               wireframeIndices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glGenVertexArrays(1, &wireframeVao);
  glBindVertexArray(wireframeVao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);  //it's the same vertex data
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireframeIbo);

    // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  // normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // color attribute
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
  glCheckError(__FILE__, __LINE__);
}

VertexType Mesh::generateVertex(const glm::vec2 position, glm::vec4 color) {
  const glm::vec2 dx(1.0, 0.0);
  const glm::vec2 dy(0.0, 1.0);

  VertexType v;
  float h = 0;
  v.position = glm::vec3(position, h);

  // We'll calculate normals once we have our triangles 
  v.normal = glm::vec3(0.0f,0.0f,0.0f);
  v.color = color;
  return v;
}

void Mesh::calculateNormals(std::vector<VertexType> &vertices, std::vector<GLuint> indices){
  // go through the indices 3 at a time
  for( int i = 0; i < indices.size() ; i+=3){
    glm::vec3 u = vertices[indices[i]].position - vertices[indices[i+1]].position;
    glm::vec3 v = vertices[indices[i+2]].position - vertices[indices[i+1]].position;
    glm::vec3 normal = glm::cross(u,v);
    
    vertices[i].normal += normal;
    vertices[i+1].normal += normal;
    vertices[i+2].normal += normal;
  }
}

std::vector<GLuint> Mesh::generateTriangularIndices(int size) {
  std::vector<GLuint> indices;
  
  // indices.resize((size+1) * (size+1)); // not sure why this wouldn't work?
  for (int y = 0; y < size; ++y) {
    for (int x = 0; x < size; ++x) {
      // first triangle
      indices.push_back((x + 0) + (size + 1) * (y + 0));
      indices.push_back((x + 1) + (size + 1) * (y + 0));
      indices.push_back((x + 1) + (size + 1) * (y + 1));

      // second triangle
      indices.push_back((x + 1) + (size + 1) * (y + 1));
      indices.push_back((x + 0) + (size + 1) * (y + 1));
      indices.push_back((x + 0) + (size + 1) * (y + 0));
    }
  }
  std::cout << "indices " << indices.size() << std::endl;
  return indices;
}

// using GL_LINES
std::vector<GLuint> Mesh::generateWireframeIndices(int size){
  std::vector<GLuint> indices;
  //across
  for(int i=0; i <= size; i++){
    for(int j=0; j < size; j++){
      indices.push_back((i * (size+1)) + j);
      indices.push_back((i * (size+1)) + j + 1);
    }
  }
  
  // down
  for(int i=0; i <= size; i++ ){
    for(int j=0; j < size; j++){
      indices.push_back((i + (size+1)*j));
      indices.push_back((1+j) * (size + 1) + i);
    }
  }
  
  return indices;
}