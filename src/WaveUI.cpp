#include "WaveUI.hpp"



#include <iostream>
#include "GLFW/glfw3.h"

static const float SPEED = 0.25f;
static const float RADIAL_VELOCITY = 10.0f;
static const float MAX_AMPLITUDE = 5.0f;
static const float MAX_HEADING = 359.9f;

WaveUI::WaveUI(const vector<shared_ptr<Wave>>& aWaves, Updatable* anUpdatable){
  waves = aWaves;
  updatable = anUpdatable;
  selectWave(0);
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

void WaveUI::processInput(GLFWwindow *window, float deltaTime){
  // manipulate wave properties
  // wave selection
  for(int i = 0; i < 10; i++ ){
    int key = GLFW_KEY_0 + i;
      if (glfwGetKey(window, key) == GLFW_PRESS){
        if (keyPressState[key] == GLFW_RELEASE) {
        std::cout << "wave " << i << " selected" << std::endl;
        selectWave(i);
        keyPressState[key] = GLFW_PRESS;
      } } else{
        keyPressState[key] = GLFW_RELEASE;
      }
  } 

  float velocity = SPEED * deltaTime;
  if(glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS){
    // decrease wave amplitude just to zero
      selectedWave->amplitude -= velocity;
      if(selectedWave->amplitude < 0){
        selectedWave->amplitude = 0;
      }
      updatable->update();
  }

  if(glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS){
      selectedWave->amplitude += velocity;
      if(selectedWave->amplitude > MAX_AMPLITUDE){
        selectedWave->amplitude = MAX_AMPLITUDE;
      }
      updatable->update();
  }
  
  if(glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS){
      float heading = selectedWave->heading + RADIAL_VELOCITY * deltaTime;
      Wave wave(selectedWave->amplitude,selectedWave->wavelength,heading,selectedWave->steepness);
      selectedWave->heading = wave.heading;
      selectedWave->direction = wave.direction;
      updatable->update();
  }

  if(glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS){
      float heading = selectedWave->heading - RADIAL_VELOCITY * deltaTime;
      Wave wave(selectedWave->amplitude,selectedWave->wavelength,heading,selectedWave->steepness);
      selectedWave->heading = wave.heading;
      selectedWave->direction = wave.direction;
      updatable->update();
  }

}