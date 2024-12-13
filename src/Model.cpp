#include "Model.hpp"

#include "asset.hpp"
#include "glError.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <glm/gtc/matrix_transform.hpp>

Model::Model(int meshSize)
    : transform(1.0),
      meshes({Mesh(meshSize)}) {
  transform =
      glm::rotate(transform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

      // we'll create a uniform buffer object here; we want to move it up to the application layer at some future point
  // we need to track our vertex shaders in order to bind their uniforms
  Shader vertexShader(SHADER_DIR "/shader.vert", GL_VERTEX_SHADER);
  ShaderProgram program({vertexShader,Shader(SHADER_DIR "/shader.frag", GL_FRAGMENT_SHADER)}); 
  shaderPrograms.push_back(program);

  GLuint handle = vertexShader.getHandle();
  GLuint index = glGetUniformBlockIndex(handle, "Matrices"); 
  glUniformBlockBinding(handle,index,0);

  glGenBuffers(1, &uboMatrices);
  glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
  glBufferData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), NULL, GL_STATIC_DRAW); 
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

  glCheckError(__FILE__, __LINE__);
}

void Model::draw(glm::mat4 view, glm::mat4 projection) {

  glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
  glBufferSubData(GL_UNIFORM_BUFFER, 0,sizeof(glm::mat4),glm::value_ptr((projection)));
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4),sizeof(glm::mat4),glm::value_ptr((view)));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  
  // for each shader
  for (int i = 0; i < meshes.size(); i++) {
    for (int j = 0; j < shaderPrograms.size(); j++) {
      ShaderProgram program = shaderPrograms[j];
      program.use();

      program.setUniform("model", transform);

      glCheckError(__FILE__, __LINE__);
      meshes[i].draw(program);
      program.unuse();  // change the naming to activate/deactivate
    }
  }
}

void Model::setTransform(glm::mat4 transform) {
  this->transform = transform;
}
