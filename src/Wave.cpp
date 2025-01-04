#include <Wave.hpp>
Wave::Wave(WaveSerialized& serialized) {
  amplitude = serialized.amplitude;
  wavelength = serialized.wavelength;
  heading = serialized.heading;
  float radians = glm::radians(heading);//TODO: rename this and the direction vector
  direction.y = sin(radians);
  direction.x = cos(radians); 
  steepness = serialized.steepness;
}

Wave::Wave(float anAmplitude, float aWavelength, float aHeading, float aSteepness){
  amplitude = anAmplitude;
  wavelength = aWavelength;
  heading = aHeading;
  float radians = glm::radians(aHeading);
  direction.y = sin(radians);
  direction.x = cos(radians);
  steepness = aSteepness;
}

float Wave::getVelocity() const {
  return 2.0f * (float)M_PI / wavelength;
}
