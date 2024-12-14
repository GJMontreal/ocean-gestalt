#include "Ocean.hpp"
#include "Mesh.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

Ocean::Ocean(int meshSize, std::vector<ShaderProgram> shaderPrograms)
    : Model(meshSize, shaderPrograms) {
  initParticles();
  resetParticles();
  waves.push_back(Wave(1,10));
}

void Ocean::draw() {
  // calculate new positions for our particles
  float time = glfwGetTime();
  moveParticles(time);
  Model::draw();
}

Ocean::~Ocean() {
  for (glm::vec3* particle : particles) {
    delete (particle);
  }
}

void Ocean::initParticles() {
  int size = meshes[0].getSize();
  for (int i = 0; i < (size +1) * (size+1); i++) {
    particles.push_back(new glm::vec3());
  }
}

void Ocean::resetParticles() {
  int size = meshes[0].getSize();
  std::vector<glm::vec3*>::iterator particle;
  particle = particles.begin();
    for (int y = 0; y <= size; ++y)
      for (int x = 0; x <= size; ++x) {
        float xx = (x - size / 2);
        float yy = (y - size / 2);
        (*particle)->x = xx;
        (*particle)->x = yy;
        (*particle)->z = 0;
        particle++;
      }
  }


void Ocean::moveParticles(float time) {
  resetParticles();

  int size = meshes[0].getSize();
  std::vector<glm::vec3*>::iterator iter;
  iter = particles.begin();
  
  for (int y = 0; y <= size; ++y){
    for (int x = 0; x <= size; ++x) {
      glm::vec3 *particle = *iter;
      glm::vec3 vec0(*particle);
      
      for(Wave wave: waves){
        float innerProd = time * wave.velocity - ( wave.k.x * vec0.x + wave.k.y * vec0.y );
        particle->z += wave.amplitude * cos(innerProd);
        // we can do some optimizations here
        particle->x += wave.amplitude * sin(innerProd) * wave.k.x / wave.kMagnitude;
        particle->y += wave.amplitude * sin(innerProd) * wave.k.y / wave.kMagnitude; 
      }
      iter++; 
    }
  }
}
