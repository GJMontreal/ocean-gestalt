#include "Mesh.hpp"

#include "asset.hpp"
#include "glError.hpp"

#include <iostream>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>

using namespace  glm;

Mesh::Mesh(int meshSize, int meshSubdivisions, glm::vec4 aColor)
{
  color = aColor;
  size = meshSize;
  subdivisions = meshSubdivisions;
  generateMesh(size, subdivisions);
}

GLuint Mesh::getVbo()const{
  return vbo;
}

std::vector<GLuint> Mesh::getTriangularIndices()const{
  return triangularMeshIndices;
}

void Mesh::drawWireframe()const{
  auto xy = size * subdivisions;
  glBindVertexArray(wireframeVao);
  glDrawElements(GL_LINES,         // mode
                 xy * (xy + 1) * 4 ,  // number of lines * number of directions * number of vertices
                 GL_UNSIGNED_INT,  // type
                 nullptr              // element array buffer offset
  );

#ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif

  glBindVertexArray(0);
}



void Mesh::draw() const {
  auto xy = size * subdivisions;
  glBindVertexArray(vao);

  glDrawElements(GL_TRIANGLES,     // mode
                 xy * xy * 2 * 3,  // count
                 GL_UNSIGNED_INT,  // type
                 nullptr           // element array buffer offset
  );

#ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif

  glBindVertexArray(0);
}

void Mesh::drawNormals()const{
  auto xy = size * subdivisions;
  glBindVertexArray(wireframeVao);
  glDrawElements(GL_LINES,         // mode
                 xy * (xy + 1) * 4 ,  // number of lines * number of directions * number of vertices
                 GL_UNSIGNED_INT,  // type
                 nullptr              // element array buffer offset
  );
  
  #ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
  #endif
  
  glBindVertexArray(0);
}

// The mesh is symmetrical in x and y
// This could be broken up a little
void Mesh::generateMesh(int meshSize, int meshSubdivisions){
  // Mesh will have vertex and normal for the moment
  std::vector<Vertex> vertices;
  auto xyVertices = meshSize * meshSubdivisions;
  float subdivisionSize = 1.0f / (float)meshSubdivisions;
//will this only work for even multiples of aSize - (maybe it should be something like subdivisions instead)
  for (int y = 0; y <= xyVertices; ++y)
    for (int x = 0; x <= xyVertices; ++x) {
      float xx = (float)x * subdivisionSize - ((float)meshSize / 2.0f);
      float yy = (float)y * subdivisionSize - ((float)meshSize / 2.0f);
      vertices.push_back(generateVertex({xx, yy}));
    }

  // generate indices for a triangular mesh
  triangularMeshIndices = generateTriangularIndices(xyVertices);
  // creation of the vertex array buffer----------------------------------------
  std::cout << "vertices=" << vertices.size() << std::endl;
  std::cout << "index=" << triangularMeshIndices.size() << std::endl;

  // vbo
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glCheckError(__FILE__, __LINE__);
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

  setVertexAttributes();
  // vao end
  glBindVertexArray(0);
  glCheckError(__FILE__, __LINE__);

  std::vector<GLuint> wireframeIndices = generateWireframeIndices(xyVertices);
  
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

  setVertexAttributes();

  glBindVertexArray(0);
  glCheckError(__FILE__, __LINE__);
}

Vertex Mesh::generateVertex(const glm::vec2 position) const{
  const glm::vec2 dx(1.0, 0.0);
  const glm::vec2 dy(0.0, 1.0);

  Vertex v;
  float h = 0;
  v.position = glm::vec3(position.x, h,position.y);
  return v;
}

std::vector<GLuint> Mesh::generateTriangularIndices(int aSize)const{
  std::vector<GLuint> indices;
  for (int y = 0; y < aSize; ++y) {
    for (int x = 0; x < aSize; ++x) {
      // first triangle
      indices.push_back((x + 0) + (aSize + 1) * (y + 0));

      indices.push_back((x + 1) + (aSize + 1) * (y + 1));
            indices.push_back((x + 1) + (aSize + 1) * (y + 0));

      // second triangle
      indices.push_back((x + 1) + (aSize + 1) * (y + 1));
   
      indices.push_back((x + 0) + (aSize + 1) * (y + 0));
         indices.push_back((x + 0) + (aSize + 1) * (y + 1));
    }
  }
  std::cout << "indices " << indices.size() << std::endl;
  return indices;
}

// using GL_LINES
std::vector<GLuint> Mesh::generateWireframeIndices(int aSize)const{
  std::vector<GLuint> indices;
  //across
  for(int i=0; i <= aSize; i++){
    for(int j=0; j < aSize; j++){
      indices.push_back((i * (aSize+1)) + j);
      indices.push_back((i * (aSize+1)) + j + 1);
    }
  }
  
  // down
  for(int i=0; i <= aSize; i++ ){
    for(int j=0; j < aSize; j++){
      indices.push_back((i + (aSize+1)*j));
      indices.push_back((1+j) * (aSize + 1) + i);
    }
  }
  
  return indices;
}

void Mesh::setVertexAttributes()const{
  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)); 
  glEnableVertexAttribArray(0);
  
  // normal attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
  glEnableVertexAttribArray(1);

  // color attribute
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tangent)));
  glEnableVertexAttribArray(2); 
}