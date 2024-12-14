#include "Ocean.hpp"
#include "Mesh.hpp"
#include "Vertex.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

Ocean::Ocean(int meshSize, std::vector<ShaderProgram> shaderPrograms)
    : Model(meshSize, shaderPrograms) {
  initParticles();
  resetParticles();
  waves.push_back(new Wave(1, 10));
  waves.push_back(new Wave(1, 13,0.3f));
  waves.push_back(new Wave(.05,2,M_PI/7));
  waves.push_back(new Wave(.05,2.5,M_PI/2 + .1));
  waves.push_back(new Wave(1.5f,12.0f,1.2f));
}

void Ocean::draw() {
  // calculate new positions for our particles
  float time = glfwGetTime();
  moveParticles(time);
  Model::draw();
}

Ocean::~Ocean() {
  // for (VertexType particle : particles) {
  //   delete (particle);
  // 
}

void Ocean::initParticles() {
  int size = meshes[0].getSize();
  for (int i = 0; i < (size + 1) * (size + 1); i++) {
    particles.resize((size+1)*(size+1));
    // particles.push_back(new VertexType());
  }
}

void Ocean::resetParticles() {
  int size = meshes[0].getSize();
  std::vector<VertexType>::iterator particle;
  particle = particles.begin();
  for (int y = 0; y <= size; ++y)
    for (int x = 0; x <= size; ++x) {
      float xx = (x - size / 2);
      float yy = (y - size / 2);
      (particle)->position.x = xx;
      (particle)->position.y = yy;
      (particle)->position.z = 0;
      (particle)->normal = glm::normalize(glm::vec3(xx, yy, 1.0));
      particle->color = glm::vec4({.7f,.7f,0.7f,0.0f});
      particle++;
    }
}

void Ocean::moveParticles(float time) {
  resetParticles();

  int size = meshes[0].getSize();
  std::vector<VertexType>::iterator iter;
  iter = particles.begin();

  for (int y = 0; y <= size; ++y) {
    for (int x = 0; x <= size; ++x) {
      VertexType particle = *iter; // does this operate on a copy then
      glm::vec3 vec0(particle.position);
      
      for (Wave* wave : waves) {
        float innerProd =
            time * wave->velocity - (wave->k.x * vec0.x + wave->k.y * vec0.y);
        iter->position.z += wave->amplitude * cos(innerProd);
        // we can do some optimizations here
        iter->position.x +=
            wave->amplitude * sin(innerProd) * wave->k.x / wave->kMagnitude;
        iter->position.y +=
            wave->amplitude * sin(innerProd) * wave->k.y / wave->kMagnitude;
      }

      //recalculate the normals
      iter++;
    }
  }
  //update the vertices
  glBindBuffer(GL_ARRAY_BUFFER, meshes[0].getVbo());
  glBufferSubData(GL_ARRAY_BUFFER,0,particles.size() * sizeof(VertexType),
               particles.data());
  glBindBuffer(GL_ARRAY_BUFFER,0);
}
