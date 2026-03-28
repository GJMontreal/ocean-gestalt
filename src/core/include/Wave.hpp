#ifndef WAVE_HPP
#define WAVE_HPP

#include "ApiValue.hpp"
#include <glm/glm.hpp>
#include <string>


using vec2 = glm::vec2;
using vec3 = glm::vec3;


class Wave;
class WaveSerialized {
 public:
  WaveSerialized() = default;
  explicit WaveSerialized(const Wave& wave);
  float heading = 0.0f;  // degrees
  float wavelength = 0.0f;
  float steepness = 1.0f;
  float amplitude = 0.0f;
  float phase = 0.0f;
};

class WaveInterface{
  public:
   void setWaveParameters(std::vector<std::shared_ptr<Wave>> waves,
                          const std::string& path,
                          const ApiValue& value);
};

class Wave {
 public:
  explicit Wave(const WaveSerialized& serialized);
  Wave() = default;
  Wave(float amplitude, float wavelength, float heading, float steepness);
  ~Wave() = default;

  void setAmplitude (float amplitude ) {this->amplitude = amplitude;};
  void setDirection (glm::vec2 direction) { this->direction = direction;};
  void setWavelength(float wavelength) { this->wavelength = wavelength;};
  void setSteepness(float steepnesss) { this->steepness = steepnesss;};
  void setPhase(float phase) { this->phase = phase;};

  vec2 direction = vec2(0.0f, 0.0f);
  float heading = 0.0f;
  float wavelength = 0.0f;
  float steepness = 1.0f;
  float amplitude = 0.0f;
  float phase = 0.0f;
  float getVelocity() const;
};

#endif
