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
    if (getIfShouldDrawWireframe()) {
      drawWireframe(uniforms);
    }

#ifndef __EMSCRIPTEN__
    if (getIfShouldDrawNormals()) {
      drawNormals(uniforms, Transform());
    }
#endif

    if (getIfShouldDrawMesh()) {
      drawMesh(uniforms, Transform());
    }
  }
}

void Model::drawNormals(Uniforms& uniforms, Transform _) {
  for (Mesh mesh : meshes) {
    auto shader = configuration->getShader("normal_shader");
    auto _guard = ShaderScope(shader);
    shader->setUniform("time", uniforms.time);
    shader->setUniform("model", getTransform());
    mesh.drawWireframe();
  }
}

void Model::drawWireframe(Uniforms& uniforms) {
  for (Mesh mesh : meshes) {
    auto shader = configuration->getShader("wireframe_shader");
    auto _guard = ShaderScope(shader);
    shader->setUniform("time", uniforms.time);
    shader->setUniform("model", getTransform());
      
#ifdef __EMSCRIPTEN__
    shader->setUniform("projection", uniforms.projection);
    shader->setUniform("view", uniforms.view);
#endif

#ifdef DEBUG_GL
    glCheckError(__FILE__, __LINE__);
#endif
    mesh.drawWireframe();
  }
}

void Model::drawMesh(Uniforms& uniforms, Transform _) {
  for (Mesh mesh : meshes) {
    auto shader = configuration->getShader("mesh_shader");
    auto _guard = ShaderScope(shader);

    shader->setUniform("time", uniforms.time);

    shader->setUniform("model", this->getTransform());
    shader->setUniform("lightPos",configuration->light->getPosition());
    shader->setUniform("viewPos",configuration->camera->getPosition()); //this will be wrong if the camera is moving up and down with the waves

#ifdef __EMSCRIPTEN__
    shader->setUniform("projection", uniforms.projection);
    shader->setUniform("view", uniforms.view);

#endif

#ifdef DEBUG_GL
    glCheckError(__FILE__, __LINE__);
#endif
  
    if (drawTriangles) {
      mesh.draw();
    }
    if (getIfShouldDrawLines()) {
      //offset the wireframe ever so slightly so it doesn't draw on top of our shaded mesh
      shader->setUniform("model", glm::translate(this->getTransform(),glm::vec3(0.f,-.1f,0.f)));
      mesh.drawWireframe();
    }
  }
}

Mesh* Model::getMesh(int index){
  return &(meshes[index]);
}

void Model::calculateNormalMatrix(const glm::mat4& modelTransform, glm::mat3& aNormalMatrix){
  aNormalMatrix = glm::transpose(glm::inverse(modelTransform));
}
