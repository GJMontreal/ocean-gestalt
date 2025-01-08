#include "WaveUI.hpp"

#include <iostream>
#include "GLFW/glfw3.h"

static const float SPEED = 0.25f;  // TODO: change this name
static const float RADIAL_VELOCITY = 10.0f;
static const float MAX_AMPLITUDE = 5.0f;
static const float MAX_HEADING = 359.9f;
static const float LONGITUDINAL_VELOCITY = 2.0f;
static const float VERTICAL_VELOCITY = 0.5;

WaveUI::WaveUI(const vector<shared_ptr<Wave>>& aWaves, Updatable* anUpdatable) {
  waves = aWaves;
  updatable = anUpdatable;
  selectWave(0);
}

void WaveUI::selectWave(int index) {
  // validate that the index is within bounds
  if (index >= 0 && index < waves.size()) {
    selectedWave = waves[index];
  } else {
    std::cout << "index " << index << " out of range" << std::endl;
  }
}

void WaveUI::processInput(GLFWwindow* window, float deltaTime) {
  for (int i = 0; i < 10; i++) {
    int key = GLFW_KEY_0 + i;
    executeIfPressed(window, key, [this,i](){
        std::cout << "wave " << i << " selected" << std::endl;
        selectWave(i);
    });
  }

  bool updateUniforms = false;
  updateUniforms = adjustAmplitude(window, deltaTime);
  updateUniforms = updateUniforms || adjustDirection(window, deltaTime);
  updateUniforms = updateUniforms || adjustWavelength(window, deltaTime);

  if(updateUniforms){
    updatable->wavesNeedUpdate = true;
  }
}
bool WaveUI::adjustAmplitude(GLFWwindow* window, float deltaTime) {
  bool updateUniforms = false;
  float velocity = SPEED * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
    // decrease wave amplitude just to zero
    selectedWave->amplitude -= velocity;
    if (selectedWave->amplitude < 0) {
      selectedWave->amplitude = 0;
    }
    updateUniforms = true;
  }

  if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
    selectedWave->amplitude += velocity;
    float max_amplitude = 3.0f * selectedWave->wavelength / 4.0f;
    if (selectedWave->amplitude > max_amplitude) {
      selectedWave->amplitude = max_amplitude;
    }
    updateUniforms = true;
  }
  return updateUniforms;
}

bool WaveUI::adjustDirection(GLFWwindow* window, float deltaTime) {
  bool updateUniforms = false;
  if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
    float heading = selectedWave->heading + RADIAL_VELOCITY * deltaTime;
    Wave wave(selectedWave->amplitude, selectedWave->wavelength, heading,
              selectedWave->steepness);
    selectedWave->heading = wave.heading;
    selectedWave->direction = wave.direction;
    updateUniforms = true;
  }

  if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) {
    float heading = selectedWave->heading - RADIAL_VELOCITY * deltaTime;
    Wave wave(selectedWave->amplitude, selectedWave->wavelength, heading,
              selectedWave->steepness);
    selectedWave->heading = wave.heading;
    selectedWave->direction = wave.direction;
    updateUniforms = true;
  }
  return updateUniforms;
}

bool WaveUI::adjustWavelength(GLFWwindow* window, float deltaTime) {
  bool updateUniforms = false;
  if (glfwGetKey(window, GLFW_KEY_APOSTROPHE) == GLFW_PRESS) {
    float wavelength = selectedWave->wavelength + LONGITUDINAL_VELOCITY * deltaTime;
    selectedWave->wavelength = wavelength;
    updateUniforms = true;
  }

  if (glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_PRESS) {
    float wavelength = selectedWave->wavelength - LONGITUDINAL_VELOCITY * deltaTime;
    selectedWave->wavelength = wavelength;
    updateUniforms = true;
  }

  return updateUniforms;
}