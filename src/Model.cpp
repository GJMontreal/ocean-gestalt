#include "Model.hpp"

#include "asset.hpp"
#include "glError.hpp"

#include <glm/gtc/matrix_transform.hpp>

Model::Model(int meshSize)
    : transform(1.0),
      meshes({Mesh(meshSize)}),
      shaderPrograms({ShaderProgram(
          {Shader(SHADER_DIR "/shader.vert", GL_VERTEX_SHADER),
           Shader(SHADER_DIR "/shader.frag", GL_FRAGMENT_SHADER)})}) {
  transform =
      glm::rotate(transform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}

void Model::draw(glm::mat4 view, glm::mat4 projection) {
  // for each shader
  for (int i = 0; i < meshes.size(); i++) {
    for (int j = 0; j < shaderPrograms.size(); j++) {
      ShaderProgram program = shaderPrograms[j];
      program.use();

      // send uniforms
      program.setUniform("projection", projection);
      program.setUniform("view", view);
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
