#ifndef __DEBUG_MATRIX_HPP
#define __DEBUG_MATRIX_HPP

#include <glm/glm.hpp>
#include <iostream>

void dumpVector(std::string name, glm::vec3 vector){
  std::cout << name << " " << vector.x << " " << vector.y << " " << vector.x << std::endl; 
}

#endif