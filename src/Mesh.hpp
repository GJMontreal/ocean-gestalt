#ifndef MESH_HPP
#define MESH_HPP

#include "Shader.hpp"
#include "Vertex.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>


class Mesh{
  public:
  Mesh(int size);
  ~Mesh();

  int getSize();
  void draw(ShaderProgram program); 

  GLuint getVbo();

  private:
    int size;
    glm::vec4 color;

    // VBO/VAO/ibo
    GLuint vao, vbo, ibo;

    void generateMesh(int size);
    VertexType generateVertex(const glm::vec2 position, glm::vec4 color);
    void generateNormals(std::vector<VertexType> &vertices, std::vector<GLuint> indices);
};

#endif
