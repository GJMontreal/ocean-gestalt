#include <Wave.hpp>

#include <cmath>

Wave::Wave(float amplitude, float wavelength, vec2 direction){
  m_amplitude = amplitude;
  // m_period = period;
  m_wavelength = wavelength;
  m_direction = direction;
  m_steepness = .75f;
  update();
}

void Wave::update(){
  m_k = 2.0f * M_PI / m_wavelength;
  m_velocity = 1.0f;
  // float velocitySqr = velocity * velocity;
  // m_k.x = velocitySqr * sin(direction);
  // m_k.y = velocitySqr * cos(direction);
  m_kMagnitude = length(m_k);
} 