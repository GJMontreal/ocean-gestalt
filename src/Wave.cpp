#include <Wave.hpp>

#include <cmath>

Wave::Wave(float amplitude, float period, float direction){
  this->amplitude = amplitude;
  this->period = period;
  this->direction = direction;
  update();
}

void Wave::update(){
  velocity = 2.0f * M_PI / period; 
} 