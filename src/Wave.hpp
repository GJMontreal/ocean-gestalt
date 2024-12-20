#ifndef WAVE_HPP
#define WAVE_HPP

#include <glm/glm.hpp>

using namespace glm;
class Wave{
  public:
    // Wave(float amplitude=0, float period=0, float direction=0);
    Wave(float amplitude, float wavelength, vec2 direction);

    void setAmplitude(float amplitude);
    // void setPeriod(float period);
    void setDirection(glm::vec2 direction);

    const float &amplitude = m_amplitude ;
    // const float &period = m_period;
    const float &wavelength = m_wavelength;
    const vec2 &direction = m_direction;
    const float &velocity = m_velocity;
    const float &k = m_k; //wavevector?
    const float &kMagnitude = m_kMagnitude;
    const float &steepness = m_steepness;
  private:
    float m_amplitude;
    float m_period;
    vec2 m_direction;
    float m_velocity;
    float m_k;
    float m_kMagnitude;
    float m_wavelength;
    float m_steepness;

    void update();

};

#endif