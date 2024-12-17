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
  void draw();  
  void drawWireframe();
  void drawNormals();

  void calculateNormals(
      std::vector<VertexType>& vertices,
      std::vector<GLuint>
          indices);  // this could also return an array of vertices that
                     // corresponds to the face normals

  GLuint getVbo();
  // we'll hang onto the mesh indices to simplify recalculating our normals
  // later
  std::vector<GLuint> getTriangularIndices();

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
  VertexType generateVertex(const glm::vec2 position, glm::vec4 color);

  std::vector<GLuint> generateTriangularIndices(
      int size);  
  std::vector<GLuint> generateWireframeIndices(int size);
  
  void setupWireframeBuffers(GLuint vba);
  void setupMeshBuffers(GLuint vba);

  void setVertexAttributes();
};

#endif
