#include "Model.hpp"

#include "asset.hpp"
#include "glError.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <glm/gtc/matrix_transform.hpp>

Model::Model(int meshSize, std::vector<ShaderProgram> shaderPrograms)
    : transform(1.0),
      meshes({Mesh(meshSize)}) {
  transform =
      glm::rotate(transform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  this->shaderPrograms = shaderPrograms;
  // this->wireframeShaderProgram = ShaderProgram({Shader(SHADER_DIR "/shader.vert", GL_VERTEX_SHADER),Shader(SHADER_DIR "/wireframe.fs", GL_FRAGMENT_SHADER)});
  // why isn't this necessary ???
  // GLuint handle = vertexShader.getHandle();
  // GLuint index = glGetUniformBlockIndex(handle, "Matrices"); 
  // glUniformBlockBinding(handle,index,0);

  glCheckError(__FILE__, __LINE__);
}

// specify different shaders for mesh, wireframe, and normals
void Model::draw() {
  // for each shader
  for (int i = 0; i < meshes.size(); i++) {
    // for each program
    wireframeShaderProgram.activate();
    wireframeShaderProgram.setUniform("model", transform);
    meshes[i].drawWireframe();
    wireframeShaderProgram.deactivate();
    
    for (int j = 0; j < shaderPrograms.size(); j++) {
      ShaderProgram program = shaderPrograms[j];
      program.activate();

      program.setUniform("model", transform);

      glCheckError(__FILE__, __LINE__);
      meshes[i].draw(); 
      meshes[i].drawWireframe();
      program.deactivate();
    }
  }
}

void Model::setTransform(glm::mat4 transform) {
  this->transform = transform;
}
