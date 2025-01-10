#include "WaveGenerator.hpp"
#include "Wave.hpp"

#include "random.hpp"

#include <glm/glm.hpp>

#include <memory>

using Random = effolkronium::random_static;

static const float DIRECTIONAL_VARIANCE = 180.0f;  // degrees
static const float STEEPNESS = 1.0f;

// Generate a set of waves with a normal distribution about the median
// for a given wave medianAmplitude / medianWavelength == amplitude / wavelength
WaveGenerator::WaveGenerator(vector<shared_ptr<Wave>>& waves,
                             float stdDev,
                             float medianWavelength,
                             float medianAmplitude) {
  auto numWaves = waves.size();
  auto wavelength = vector<float>(numWaves);
  for (int i = 0; i < numWaves / 2; i++) {  // only works for ^2 numWaves
    auto deviation = Random::get<float>(0.f, 1.f) * stdDev + stdDev / 2.0f;
    wavelength[2 * i] = medianWavelength + deviation;
    wavelength[2 * i + 1] = medianWavelength - deviation;
  }

  // now amplitudes
  auto amplitude = vector<float>(numWaves);
  auto amplitudeToWavelength = medianAmplitude / medianWavelength;
  for (int i = 0; i < numWaves; i++) {
    amplitude[i] = wavelength[i] * amplitudeToWavelength;
  }

  // choose a random wind direction
  auto windDirection = Random::get<float>(0, 2.f * (float)M_PI);
  for (int i = 0; i < numWaves; i++) {
    auto serialized = WaveSerialized();
    serialized.wavelength = wavelength[i];
    serialized.amplitude = amplitude[i];
    auto directionVariance =
        (Random::get<float>(0.f, 1.f) * DIRECTIONAL_VARIANCE) -
        (DIRECTIONAL_VARIANCE / 2.0f);
    serialized.heading = glm::degrees(windDirection) + directionVariance;

    serialized.steepness = STEEPNESS;
    waves[i] = std::make_shared<Wave>(serialized);
  }
}