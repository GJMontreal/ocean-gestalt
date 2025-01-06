#include "WaveGenerator.hpp"
#include "Wave.hpp"

#include <glm/glm.hpp>
#include <memory>

static const float DIRECTIONAL_VARIANCE = 36.0f; // degrees
static const float AMPLITUDE_MEDIAN = 1.0f;
static const float WAVELENGTH_MEDIAN = 1.0f;
static const float MAX_AMPLITUDE = 5.0;
static const float MAX_WAVELENGTH = 20.0f;
static const float MIN_WAVELENGTH = 1.0f;
static const float STEEPNESS = 1.0f;

WaveGenerator::WaveGenerator(vector<shared_ptr<Wave>>& waves) {
  // Randomly create a distribution of waves
  auto numWaves = waves.size();
  
  // choose a random wind direction
  auto windDirection = (float)random() / (float)RAND_MAX * 2 * M_PI; // radians

  for (int i = 0; i < numWaves; i++) {
    auto serialized = WaveSerialized();
    serialized.wavelength = (float)random() / (float)RAND_MAX * (MAX_WAVELENGTH - MIN_WAVELENGTH) + MIN_WAVELENGTH ;
    serialized.amplitude = (float)random() / (float)RAND_MAX * 2.0f/3.0f * serialized.wavelength;
    auto directionVariance =
        ((float)random() / (float)RAND_MAX * DIRECTIONAL_VARIANCE) -
        (DIRECTIONAL_VARIANCE / 2.0f);
    serialized.heading = (float)glm::degrees(windDirection) + directionVariance;

    serialized.steepness = STEEPNESS;
    waves[i] = std::make_shared<Wave>(serialized);
  }
}
