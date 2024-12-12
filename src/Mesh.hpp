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

  void draw(glm::mat4 view, glm::mat4 projection); // should these be pointers?
  
  private:
    int size;
    glm::vec4 color;
    glm::mat4 model = glm::mat4(1.0);

    // VBO/VAO/ibo
    GLuint vao, vbo, ibo;

    // this should really be a list of shader programs
    // shader
    Shader vertexShader;
    Shader fragmentShader;
    ShaderProgram shaderProgram;
    
    void generateMesh(int size);
    VertexType generateVertex(const glm::vec2 position, glm::vec4 color);
};

#endif
