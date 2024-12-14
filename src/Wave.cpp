#include <Wave.hpp>

#include <cmath>

Wave::Wave(float amplitude, float period, float direction){
  m_amplitude = amplitude;
  m_period = period;
  m_direction = direction;
  update();
}

void Wave::update(){
  m_velocity = 2.0f * M_PI / period;
  float velocitySqr = velocity * velocity;
  m_k.x = velocitySqr * sin(direction);
  m_k.y = velocitySqr * cos(direction);
  m_kMagnitude = glm::length(m_k);
} 