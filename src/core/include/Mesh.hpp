#pragma once

#include "Shader.hpp"
#include "Vertex.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

class Mesh {
 public:
  Mesh(int size, int subdivisions, glm::vec4 aColor = glm::vec4({.01f, .15f, .210f, 0.0f})); // this colour should come from the configuration
  ~Mesh() = default;

  // So that we can specify different shaders for each
  void draw()const;  
  void drawWireframe()const;
  void drawNormals()const;

  void calculateNormals(
      std::vector<Vertex>& vertices,
      std::vector<GLuint>
          indices)const;  // this could also return an array of vertices that
                     // corresponds to the face normals

  GLuint getVbo()const; //why
  
  std::vector<GLuint> getTriangularIndices()const;

 private:
  int size;  // x and y are symmetrical
  int subdivisions; 
  glm::vec4 color;

  // Our vertices are shared between the triangular mesh and line strips
  GLuint vao;
  GLuint vbo;
  GLuint ibo;

  GLuint wireframeVao;
  GLuint wireframeIbo;
  GLuint meshVao;

  std::vector<GLuint> triangularMeshIndices;

  void generateMesh(int size, int subdivisions);
  Vertex generateVertex(const glm::vec2 position) const;

  std::vector<GLuint> generateTriangularIndices(
      int size)const;  
  std::vector<GLuint> generateWireframeIndices(int size)const;
  
  void setupWireframeBuffers(GLuint vba);
  void setupMeshBuffers(GLuint vba);

  void setVertexAttributes()const;
};
