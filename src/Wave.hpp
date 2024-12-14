#ifndef WAVE_HPP
#define WAVE_HPP

#include <glm/glm.hpp>

class Wave{
  public:
    Wave(float amplitude=0, float period=0, float direction=0);

    void setAmplitude(float amplitude);
    void setPeriod(float period);
    void setDirection(float direction);

    const float &amplitude = m_amplitude ;
    const float &period = m_period;
    const float &direction = m_direction;
    const float &velocity = m_velocity;
    const glm::vec2 &k = m_k; //wavevector
    const float &kMagnitude = m_kMagnitude;
  private:
    float m_amplitude;
    float m_period;
    float m_direction;
    float m_velocity;
    glm::vec2 m_k;
    float m_kMagnitude;

    void update();

};

#endif