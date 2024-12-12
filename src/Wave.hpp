#ifndef WAVE_HPP
#define WAVE_HPP

class Wave{
  public:
    Wave(float amplitude, float period, float direction);

    void setAmplitude(float amplitude);
    void setPeriod(float period);
    void setDirection(float direction);

  private:
    float amplitude;
    float period;
    float direction;
    float velocity;

    void update();

};

#endif