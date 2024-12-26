#ifndef MESH_HPP
#define MESH_HPP

#include "Shader.hpp"
#include "Vertex.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

struct NormalVertices{
  std::vector<VertexType> vertices;
  std::vector<GLuint> indices;
};
class Mesh {
 public:
  Mesh(int size, glm::vec4 aColor = glm::vec4({.01f, .15f, .210f, 0.0f}));
  ~Mesh() = default;

  int getSize()const;
  
  // So that we can specify different shaders for each
  void draw()const;  
  void drawWireframe()const;
  void drawNormals()const;

  void calculateNormals(
      std::vector<VertexType>& vertices,
      std::vector<GLuint>
          indices)const;  // this could also return an array of vertices that
                     // corresponds to the face normals

  GLuint getVbo()const;
  // we'll hang onto the mesh indices to simplify recalculating our normals
  // later
  std::vector<GLuint> getTriangularIndices()const;

  NormalVertices generateNormalVertices(const std::vector<VertexType>& vertices)const;

 private:
  int size;
  glm::vec4 color;
  // Our vertices are shared between the triangular mesh and line strips
  GLuint vao;
  GLuint vbo;
  GLuint ibo;

  GLuint wireframeVao;
  GLuint wireframeIbo;
  GLuint meshVao;

  GLuint normalsVao;
  GLuint normalsVbo;
  GLuint normalsIbo;

  std::vector<GLuint> triangularMeshIndices;

  void generateMesh(int size);
  VertexType generateVertex(const glm::vec2 position, const glm::vec4& color)const;

  std::vector<GLuint> generateTriangularIndices(
      int size)const;  
  std::vector<GLuint> generateWireframeIndices(int size)const;
  
  void setupWireframeBuffers(GLuint vba);
  void setupMeshBuffers(GLuint vba);

  void setVertexAttributes()const;
};

#endif
