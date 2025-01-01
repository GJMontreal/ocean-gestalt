#include "Debug.hpp"

#include <iostream>

void dumpVector(const std::string& name, glm::vec3 vector){
  std::cout << name << " " << vector.x << " " << vector.y << " " << vector.z << std::endl; 
}