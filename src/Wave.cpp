#include <Wave.hpp>

#include <iostream>

using std::cout;
using std::endl;

WaveSerialized::WaveSerialized(const Wave& wave){
  amplitude = wave.amplitude;
  wavelength = wave.wavelength;
  heading = wave.heading;
  steepness = wave.steepness;
}

Wave::Wave(const WaveSerialized& serialized) {
  std::cout << "constructing wave from serialized" << std::endl;
  amplitude = serialized.amplitude;
  wavelength = serialized.wavelength;
  heading = serialized.heading;
  float radians = glm::radians(heading);
  direction.y = sin(radians);
  direction.x = cos(radians); 
  steepness = serialized.steepness;
}

Wave::Wave(){
  std::cout << "constructing default wave" <<std::endl;
}

// I don't think this is used
Wave::Wave(float anAmplitude, float aWavelength, float aHeading, float aSteepness){
  amplitude = anAmplitude;
  wavelength = aWavelength;
  heading = aHeading;
  float radians = glm::radians(aHeading);
  direction.y = sin(radians);
  direction.x = cos(radians);
  steepness = aSteepness;
}

Wave::~Wave(){
  cout << "deleting wave" << endl;
}

float Wave::getVelocity() const {
  return 2.0f * (float)M_PI / wavelength;
}
