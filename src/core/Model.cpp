#include "Model.hpp"
#include "Configuration.hpp"
#include "Wave.hpp"
#include "Light.hpp"
#include "asset.hpp"
#include "glError.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Model::Model(std::shared_ptr<Configuration> configuration)
    : meshes({Mesh(configuration->meshSize,configuration->meshSubdivisions,configuration->meshColor)}), Drawable(glm::vec3(0.0f,0.0f,0.0f),configuration) {
  this->configuration = configuration;
  calculateNormalMatrix(getTransform(), normalMatrix);
}

// specify different shaders for mesh, wireframe, and normals
void Model::draw(Uniforms& uniforms) {
{
    if (shouldDrawWireframe) {
      drawWireframe(uniforms);
    }

#ifndef __EMSCRIPTEN__
    if (shouldDrawNormals) {
      drawNormals(uniforms,glm::mat4(1.f));
    }
#endif

    if (shouldDrawMesh) {
      drawMesh(uniforms,glm::mat4(1.f));
    }
  }
}

void Model::drawNormals(Uniforms& uniforms, glm::mat4 _) {
  for (Mesh mesh : meshes) {
    configuration->normalShader->activate();
    configuration->normalShader->setUniform("time", uniforms.time);
    configuration->normalShader->setUniform("model", getTransform());
    mesh.drawWireframe();
    configuration->normalShader->deactivate();
  }
}

void Model::drawWireframe(Uniforms& uniforms) {
  for (Mesh mesh : meshes) {
    configuration->wireframeShader->activate();

    // be sure to rebind textures
    configuration->wireframeShader->setUniform("time", uniforms.time);
    configuration->wireframeShader->setUniform("model", getTransform());

#ifdef __EMSCRIPTEN__
    configuration->wireframeShader->setUniform("projection",
                                               uniforms.projection);
    configuration->wireframeShader->setUniform("view", uniforms.view);
#endif

#ifdef DEBUG_GL
    glCheckError(__FILE__, __LINE__);
#endif
    mesh.drawWireframe();
    configuration->wireframeShader->deactivate();
  }
}

void Model::drawMesh(Uniforms& uniforms, glm::mat4 _) {
  for (Mesh mesh : meshes) {
    configuration->meshShader->activate();
    configuration->meshShader->setUniform("time", uniforms.time);

    // these next three don't need to be set each pass
    configuration->meshShader->setUniform("model", this->getTransform());
    configuration->meshShader->setUniform("lightPos",
                                          configuration->light->getPosition());
    configuration->meshShader->setUniform("viewPos",
                                          configuration->camera->getPosition()); //this will be wrong if the camera is moving up and down with the waves

#ifdef __EMSCRIPTEN__
    configuration->meshShader->setUniform("projection", uniforms.projection);
    configuration->meshShader->setUniform("view", uniforms.view);

#endif

#ifdef DEBUG_GL
    glCheckError(__FILE__, __LINE__);
#endif
  
    if (drawTriangles) {
      mesh.draw();
    }
    if (drawLines) {
      //offset the wireframe ever so slightly so it doesn't draw on top of our shaded mesh
      configuration->meshShader->setUniform("model", glm::translate(this->getTransform(),glm::vec3(0.f,-.1f,0.f)));
      mesh.drawWireframe();
    }
    configuration->meshShader->deactivate();
  }
}

Mesh* Model::getMesh(int index){
  return &(meshes[index]);
}

void Model::toggleDrawNormals(){
  shouldDrawNormals = !shouldDrawNormals;
}

void Model::toggleDrawWireframe(){
  shouldDrawWireframe = !shouldDrawWireframe;
}

void Model::toggleDrawMesh(){
  shouldDrawMesh = !shouldDrawMesh;
}

void Model::toggleRunning(){
  running = !running;
}

void Model::toggleDrawLines(){
  drawLines = !drawLines;
}

void Model::toggleDrawTriangles(){
  drawTriangles = !drawTriangles;
}

bool Model::isRunning()const{
  return running;
}

void Model::calculateNormalMatrix(const glm::mat4& modelTransform, glm::mat3& aNormalMatrix){
  aNormalMatrix = glm::transpose(glm::inverse(modelTransform));
}
