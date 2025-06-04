#ifndef __WAVE_GENERATOR_HPP
#define __WAVE_GENERATOR_HPP

#include "Wave.hpp"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

class WaveGenerator {
 public:
  WaveGenerator(vector<shared_ptr<Wave>>& waves,
                float stdDev,
                float medianWavelength,
                float medianAmplitude);
};

#endif
