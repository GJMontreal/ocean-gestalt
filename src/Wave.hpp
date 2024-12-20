#ifndef WAVE_HPP
#define WAVE_HPP

#include <glm/glm.hpp>

using namespace glm;
class Wave{
  public:
    Wave(float amplitude = 0.0f, float wavelength = 0.0f, vec2 direction = vec2(0.0f,0.0f), float steepness = 1.0f);

    void setAmplitude(float amplitude);
    void setDirection(glm::vec2 direction);
    void setWavelength(float wavelength);
    
    float getAmplitude()const;
    vec2 getDirection()const;
    float getWavelength()const;
    float getVelocity()const;
    float getSteepness()const;

  private:
    float amplitude;
    float period;
    vec2 direction;
    float wavelength;
    float steepness;
};

#endif