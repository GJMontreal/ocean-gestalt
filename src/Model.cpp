#include "Model.hpp"
#include "Wave.hpp"

#include "asset.hpp"
#include "glError.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

Model::Model(int meshSize, const std::vector<ShaderProgram>& shaderPrograms, std::shared_ptr<Camera> camera)
    : transform(1.0),
      meshes({Mesh(meshSize)}) {
      this->camera = camera;
  transform =
      glm::rotate(transform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  this->shaderPrograms = shaderPrograms;

  glCheckError(__FILE__, __LINE__);
}
void Model::beginDrawing(ShaderProgram &program, Uniforms& uniforms){

}

// specify different shaders for mesh, wireframe, and normals
void Model::draw(Uniforms& uniforms) {
  glm::vec3 lightPos(0.0f, 0.0f, 10.0f); // what coordinate system is this in

  for(Mesh mesh: meshes){
    if(drawWireframe){
      ShaderProgram program = wireframeShaderProgram;
      program.activate();
      beginDrawing(program, uniforms);
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
      beginDrawing(program, uniforms);
      program.setUniform("model", transform);
      // program.setUniform("viewPos",camera->Position);
      // program.setUniform("lightPos", lightPos);
      

       #ifdef __EMSCRIPTEN__
      program.setUniform("projection", uniforms.projection);
      program.setUniform("view", uniforms.view);
      #endif
      glCheckError(__FILE__, __LINE__);

      // mesh.draw(); 
      mesh.drawWireframe();   
    
      program.deactivate();
    }
  }
}

Mesh* Model::getMesh(int index){
  return &(meshes[index]);
}

ShaderProgram* Model::getShaderProgram(int index){
  return &(shaderPrograms[index]);
}

void Model::setTransform(const glm::mat4& aTransform){
  transform = aTransform;
}

void Model::toggleDrawNormals(){
  drawNormals = !drawNormals;
}

void Model::toggleDrawWireframe(){
  drawWireframe = !drawWireframe;
}

void Model::toggleRunning(){
  running = !running;
}

bool Model::isRunning()const{
  return running;
}

bool Model::shouldDrawNormals()const{
  return drawNormals;
}

bool Model::shouldDrawWireframe()const{
  return drawWireframe;
}