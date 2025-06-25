#include "Wave.hpp"

#include "UniformValue.hpp"

#include <iostream>
#include <sstream>


WaveSerialized::WaveSerialized(const Wave& wave){
  amplitude = wave.amplitude;
  wavelength = wave.wavelength;
  heading = wave.heading;
  steepness = wave.steepness;
}

Wave::Wave(const WaveSerialized& serialized) {
  amplitude = serialized.amplitude;
  wavelength = serialized.wavelength;
  heading = serialized.heading;
  float radians = glm::radians(heading);
  direction.y = sin(radians);
  direction.x = cos(radians); 
  steepness = serialized.steepness;
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

float Wave::getVelocity() const {
  return 2.0f * (float)M_PI / wavelength;
}

  void WaveInterface::setWaveParameters(std::vector<std::shared_ptr<Wave>> waves,
                                      const std::string& path,
                                      const ApiValue& value) {
    std::cout << "Wave Interface " << path << std::endl;

    // Tokenize by '.'
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string token;
    while (std::getline(ss, token, '.')) {
        parts.push_back(token);
    }

    // Expect something like "wave[2].amplitude"
    if (parts.size() < 2)  {
        std::cerr << "Invalid path format: " << path << std::endl;
        return;
    }

    const std::string& wavePart = parts[0]; // "wave[2]"
    size_t lb = wavePart.find('[');
    size_t rb = wavePart.find(']');

    if (lb == std::string::npos || rb == std::string::npos || rb <= lb + 1) {
        std::cerr << "Invalid wave index format: " << wavePart << std::endl;
        return;
    }

    int index = std::stoi(wavePart.substr(lb + 1, rb - lb - 1));
    if (index < 0 || index >= static_cast<int>(waves.size())) {
        std::cerr << "Wave index out of bounds: " << index << std::endl;
        return;
    }

    const std::string& param = parts[1];
    auto const& wave = waves[index];
    auto val = apiToUniform(value);

    if (param == "amplitude") {
        wave->setAmplitude(std::get<float>(val));
    } else if (param == "wavelength") {
        wave->setWavelength(std::get<float>(value));
    } else if (param == "steepness") {
        wave->setSteepness(std::get<float>(value));
    } else if (param == "direction") {
        wave->setDirection(glm::dvec2(std::get<glm::vec3>(val)));
    } else {
        std::cerr << "Unknown wave parameter: " << param << std::endl;
    }
}
