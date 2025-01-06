#ifndef __WAVE_GENERATOR_HPP
#define __WAVE_GENERATOR_HPP

#include "Wave.hpp"

#include <memory>
#include <vector>

using std::vector;
using std::shared_ptr;

class WaveGenerator {
 public:
  explicit WaveGenerator(vector<shared_ptr<Wave>>& waves);
};

#endif
