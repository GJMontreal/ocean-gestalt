#ifndef WAVE_HPP
#define WAVE_HPP

#include <glm/glm.hpp>

using vec2 = glm::vec2;
class Wave;
class WaveSerialized {
 public:
  WaveSerialized() = default;
  explicit WaveSerialized(const Wave& wave);
  float heading = 0.0f;  // degrees
  float wavelength = 0.0f;
  float steepness = 1.0f;
  float amplitude = 0.0f;
};


class Wave {
 public:
  explicit Wave(const WaveSerialized& serialized);
  Wave() = default;
  Wave(float amplitude, float wavelength, float heading, float steepness);
  vec2 direction = vec2(0.0f, 0.0f);
  float heading = 0.0f;
  float wavelength = 0.0f;
  float steepness = 1.0f;
  float amplitude = 0.0f;

  float getVelocity() const;
};

#endif
