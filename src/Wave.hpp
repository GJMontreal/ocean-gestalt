#ifndef WAVE_HPP
#define WAVE_HPP

#include <glm/glm.hpp>

using vec2 = glm::vec2;

class Wave {
 public:
  Wave(float amplitude = 0.0f,
       float wavelength = 0.0f,
       vec2 direction = vec2(0.0f, 0.0f),
       float steepness = 1.0f);

  vec2 direction;
  float wavelength;
  float steepness;
  float amplitude;


  float getVelocity() const;
};

#endif