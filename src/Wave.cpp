#include <Wave.hpp>

// #include <cmath>

Wave::Wave(float aAmplitude, float aWavelength, vec2 aDirection, float aSteepness){
  amplitude = aAmplitude;
  wavelength = aWavelength;
  direction = aDirection;
  steepness = aSteepness;
}

float Wave::getVelocity()const{
  return 2.0f * (float)M_PI / wavelength;
}

float Wave::getAmplitude()const{
  return amplitude;
}

float Wave::getWavelength()const{
  return wavelength;
}

float Wave::getSteepness()const{
  return steepness;
}

vec2 Wave::getDirection()const{
  return direction;
}
