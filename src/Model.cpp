#include "Model.hpp"

#include "asset.hpp"
#include "glError.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <glm/gtc/matrix_transform.hpp>

Model::Model(int meshSize, std::vector<ShaderProgram> shaderPrograms, Camera *camera)
    : transform(1.0),
      meshes({Mesh(meshSize)}) {
      this->camera = camera;
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
void Model::draw(Uniforms uniforms) {
  glm::vec3 lightPos(0.0f, 0.0f, 10.0f); // what coordinate system is this in

  // for each shader
  for(Mesh mesh: meshes){
    if(drawWireframe){
      ShaderProgram program = wireframeShaderProgram;
      program.activate();
      program.setUniform("model", transform);
      #ifdef __EMSCRIPTEN__
      program.setUniform("projection", uniforms.projection);
      program.setUniform("view", uniforms.view);
      #endif
      glCheckError(__FILE__, __LINE__);
      mesh.drawWireframe();
      program.deactivate();
    }

  #ifndef __EMSCRIPTEN__
    if(drawNormals){
      normalShader.activate();
      normalShader.setUniform("model",transform);
      mesh.draw();
      normalShader.deactivate();
    }
  #endif

  for(ShaderProgram program: shaderPrograms){
      program.activate();

      program.setUniform("model", transform);
      program.setUniform("viewPos",camera->Position);
      program.setUniform("lightPos", lightPos);
       #ifdef __EMSCRIPTEN__
      program.setUniform("projection", uniforms.projection);
      program.setUniform("view", uniforms.view);
      #endif
      glCheckError(__FILE__, __LINE__);

      mesh.draw(); 
      mesh.drawWireframe();   
    
      program.deactivate();
    }
  }
}

void Model::setTransform(glm::mat4 transform) {
  this->transform = transform;
}
