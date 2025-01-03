#include "WaveUI.hpp"

#include <iostream>

WaveUI::WaveUI(const vector<shared_ptr<Wave>>& aWaves){
  waves = aWaves;
}

void WaveUI::selectWave(int index){
  // validate that the index is within bounds
  if(index >= 0 && index < waves.size()){
    selectedWave = waves[index];
  }else{
    // maybe we should throw an exception
    std::cout << "index " << index << " out of range" << std::endl;
  }
}
