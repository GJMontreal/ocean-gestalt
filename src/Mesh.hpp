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
  Mesh(int size);
  ~Mesh();

  int getSize();
  
  // So that we can specify different shaders for each
  const void draw();  
  const void drawWireframe();
  const void drawNormals();

  const void calculateNormals(
      std::vector<VertexType>& vertices,
      std::vector<GLuint>
          indices);  // this could also return an array of vertices that
                     // corresponds to the face normals

  GLuint getVbo();
  // we'll hang onto the mesh indices to simplify recalculating our normals
  // later
  const std::vector<GLuint> getTriangularIndices();

  glm::vec4 color;
  
  NormalVertices generateNormalVertices(std::vector<VertexType> vertices);

 private:
  int size;

  // Our vertices and elements are shared between the triangular mesh and line
  // strips
  GLuint vao, vbo, ibo;

  GLuint wireframeVao, wireframeIbo;
  GLuint meshVao;
  GLuint normalsVao, normalsVbo, normalsIbo;

  std::vector<GLuint> triangularMeshIndices;

  void generateMesh(int size);
  const VertexType generateVertex(const glm::vec2 position, glm::vec4 color);

  const std::vector<GLuint> generateTriangularIndices(
      int size);  
  const std::vector<GLuint> generateWireframeIndices(int size);
  
  const void setupWireframeBuffers(GLuint vba);
  const void setupMeshBuffers(GLuint vba);

  const void setVertexAttributes();
};

#endif
