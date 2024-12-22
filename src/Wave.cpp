#include <Wave.hpp>

Wave::Wave(float aAmplitude,
           float aWavelength,
           vec2 aDirection,
           float aSteepness) {
  amplitude = aAmplitude;
  wavelength = aWavelength;
  direction = aDirection;
  steepness = aSteepness;
}

float Wave::getVelocity() const {
  return 2.0f * (float)M_PI / wavelength;
}
