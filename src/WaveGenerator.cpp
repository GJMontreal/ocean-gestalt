#include "WaveGenerator.hpp"
#include "Wave.hpp"

#include <glm/glm.hpp>
#include <memory>


static const float DIRECTIONAL_VARIANCE = 180.0f;  // degrees
static const float MAX_WAVELENGTH = 20.0f;
static const float MIN_WAVELENGTH = 1.0f;
static const float STEEPNESS = 1.0f;

WaveGenerator::WaveGenerator(vector<shared_ptr<Wave>>& waves) {
  // Randomly create a distribution of waves
  auto numWaves = waves.size();

  // choose a random wind direction
  auto windDirection = (float)random() / (float)RAND_MAX * 2 * M_PI;  // radians

  for (int i = 0; i < numWaves; i++) {
    auto serialized = WaveSerialized();
    serialized.wavelength =
        (float)random() / (float)RAND_MAX * (MAX_WAVELENGTH - MIN_WAVELENGTH) +
        MIN_WAVELENGTH;
    serialized.amplitude =
        (float)random() / (float)RAND_MAX * 2.0f / 3.0f * serialized.wavelength;
    auto directionVariance =
        ((float)random() / (float)RAND_MAX * DIRECTIONAL_VARIANCE) -
        (DIRECTIONAL_VARIANCE / 2.0f);
    serialized.heading = (float)glm::degrees(windDirection) + directionVariance;

    serialized.steepness = STEEPNESS;
    waves[i] = std::make_shared<Wave>(serialized);
  }
}

// Generate a set of waves with a normal distribution about the median
// for a given wave medianAmplitude / medianWavelength == amplitude / wavelength
WaveGenerator::WaveGenerator(vector<shared_ptr<Wave>>& waves,
                             float stdDev,
                             float medianWavelength,
                             float medianAmplitude) {
  auto numWaves = waves.size();
  auto wavelength = vector<float>(numWaves);
  for (int i = 0; i < numWaves / 2; i++) {  // only works for ^2 numWaves
    auto deviation = (float)random() / (float)RAND_MAX * stdDev + stdDev / 2.0f;
    wavelength[2 * i] = medianWavelength + deviation;
    wavelength[2 * i + 1] = medianWavelength - deviation;
  }

  // now amplitudes
  auto amplitude = vector<float>(numWaves);
  auto amplitudeToWavelength = medianAmplitude / medianWavelength;
  for( int i = 0; i < numWaves; i++){
    amplitude[i] = wavelength[i] * amplitudeToWavelength;
  }
 
  // choose a random wind direction
  auto windDirection = (float)random() / (float)RAND_MAX * 2 * M_PI;  // radians
  for (int i = 0; i < numWaves; i++) {
    auto serialized = WaveSerialized();
    serialized.wavelength =
        wavelength[i];
    serialized.amplitude = amplitude[i];
    auto directionVariance =
        ((float)random() / (float)RAND_MAX * DIRECTIONAL_VARIANCE) -
        (DIRECTIONAL_VARIANCE / 2.0f);
    serialized.heading = (float)glm::degrees(windDirection) + directionVariance;

    serialized.steepness = STEEPNESS;
    waves[i] = std::make_shared<Wave>(serialized);
  }  
}