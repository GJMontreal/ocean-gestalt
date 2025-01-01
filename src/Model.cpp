#include "Model.hpp"
#include "Wave.hpp"

#include "asset.hpp"
#include "glError.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

Model::Model(int meshSize, const std::vector<ShaderProgram>& shaderPrograms, std::shared_ptr<Camera> camera)
    : transform(1.0),
      meshes({Mesh(meshSize)}) {
      // this->camera = camera;
  transform =
      glm::rotate(transform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  // this->shaderPrograms = shaderPrograms;

  glCheckError(__FILE__, __LINE__);
}

Model::Model(int meshSize, std::shared_ptr<Configuration> configuration)
    : transform(1.0), meshes({Mesh(meshSize)}) {
  this->configuration = configuration;
  transform =
      glm::rotate(transform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  calculateNormalMatrix(transform, normalMatrix);
}

void Model::beginDrawing(std::shared_ptr<ShaderProgram> program, Uniforms& uniforms){

}

// specify different shaders for mesh, wireframe, and normals
void Model::draw(Uniforms& uniforms) {
  glm::vec3 lightPos(0.0f, 5.0f, -40.0f); // world coordinates?
  // lightPos.x = 1.0f + sin(uniforms.time) * 2.0f;
  // lightPos.y = sin(uniforms.time / 2.0f) * 1.0f;

  for(Mesh mesh: meshes){
    if(drawWireframe){
      configuration->wireframeShader->activate();
      // beginDrawing(configuration->wireframeShader, uniforms);
      // can't we just set the model transform once?
      // configuration->wireframeShader->setUniform("model",transform);
      configuration->wireframeShader->setUniform("time",uniforms.time);
      configuration->wireframeShader->setUniform("model", transform);
      configuration->wireframeShader->setUniform("normalMatrix",normalMatrix);
      #ifdef __EMSCRIPTEN__
      configuration->wireframeShader->setUniformsetUniform("projection", uniforms.projection);
      configuration->wireframeShader->setUniformsetUniform("view", uniforms.view);
      #endif
      glCheckError(__FILE__, __LINE__);
      mesh.drawWireframe();
      configuration->wireframeShader->deactivate();
    }

  #ifndef __EMSCRIPTEN__
// TO DO: implement diagnostic/normal geometry shader 
    if(drawNormals){
      configuration->normalShader->activate();
      configuration->normalShader->setUniform("time",uniforms.time);
      configuration->normalShader->setUniform("model", transform);
      configuration->normalShader->setUniform("normalMatrix",normalMatrix);
      mesh.drawWireframe();
      configuration->normalShader->deactivate();
    }
  #endif

  if(drawMesh){
      configuration->meshShader->activate();

      configuration->meshShader->setUniform("time",uniforms.time);
      // these next three don't need to be set each pass
      configuration->meshShader->setUniform("model", transform);
      configuration->meshShader->setUniform("normalMatrix",normalMatrix);
      configuration->meshShader->setUniform("lightPos", configuration->lightPosition);

      configuration->meshShader->setUniform("viewPos",configuration->camera->Position);
      

       #ifdef __EMSCRIPTEN__
      
      configuration->meshShader->setUniform("lightPos", lightPos);setUniform("projection", uniforms.projection);
      
      configuration->meshShader->setUniform("lightPos", lightPos);etUniform("view", uniforms.view);
      #endif
      glCheckError(__FILE__, __LINE__);

      mesh.draw(); 
      mesh.drawWireframe();   
    
      configuration->meshShader->deactivate();
    }
  }
}

Mesh* Model::getMesh(int index){
  return &(meshes[index]);
}

void Model::toggleDrawNormals(){
  drawNormals = !drawNormals;
}

void Model::toggleDrawWireframe(){
  drawWireframe = !drawWireframe;
}

void Model::toggleDrawMesh(){
  drawMesh = !drawMesh;
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
void Model::calculateNormalMatrix(const glm::mat4& modelTransform, glm::mat3& aNormalMatrix){
  aNormalMatrix = glm::transpose(glm::inverse(modelTransform));
}
