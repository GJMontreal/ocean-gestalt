#ifndef __WAVE_UI_HPP
#define __WAVE_UI_HPP

#include "Wave.hpp"

#include "InputProcessor.hpp"
#include "KeyExecutable.hpp"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::weak_ptr;
using std::vector;

class WaveUI: public InputProcessor, public KeyExecutable {
  public:
  explicit WaveUI(const vector<shared_ptr<Wave>>& waves);

  void processInput(GLFWwindow *window, float deltaTime) override;
  
  void selectWave(int index);

  vector<shared_ptr<Wave>> waves;
  shared_ptr<Wave> selectedWave;
  
  weak_ptr<Updatable> updatable;

  private:
  bool adjustDirection(GLFWwindow* window, float deltaTime);
  bool adjustAmplitude(GLFWwindow* window, float deltaTime);
  bool adjustWavelength(GLFWwindow* window, float deltaTime);
};
#endif
