#ifndef MESH_HPP
#define MESH_HPP

#include "Shader.hpp"
#include "Vertex.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

class Mesh {
 public:
  Mesh(int size);
  ~Mesh();

  int getSize();
  void draw(
      ShaderProgram program);  // Not certain we actually need the shaderprogram
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

  bool drawWireframe = true;
  bool drawMesh = false;

 private:
  int size;

  // Our vertices and elements are shared between the triangular mesh and line
  // strips
  GLuint vao, vbo, ibo;

  GLuint wireframeVao, wireframeIbo;
  GLuint meshVao;
  GLuint normalsVao, NormalsVbo, NormalsIbo;

  std::vector<GLuint> triangularMeshIndices;

  void generateMesh(int size);
  VertexType generateVertex(const glm::vec2 position, glm::vec4 color);

  std::vector<GLuint> generateTriangularIndices(
      int size);  // is it possible that our mix of stack and heap is causing us
                  // trouble?
  std::vector<GLuint> generateWireframeIndices(int size);
};

#endif
