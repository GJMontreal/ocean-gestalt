#ifndef __WAVE_UI_HPP
#define __WAVE_UI_HPP

#include "Wave.hpp"

#include <memory>
#include <vector>

using namespace std;

class WaveUI {
  public:
  explicit WaveUI(const vector<shared_ptr<Wave>>& waves);

  void selectWave(int index);

  vector<shared_ptr<Wave>> waves;
  shared_ptr<Wave> selectedWave;
};
#endif
