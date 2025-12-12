#pragma once

#include <cstdlib> 

class ADSREnvelope {
 public:
  enum class State { Idle, Attack, Decay, Sustain, Release } state;

  void trigger(float a, float d, float s, float r);
  void update(float dt);
  void releaseEnv();

  float getValue() const { return value; }
  bool isActive() const { return active; }

 private:
  float attack;
  float decay;
  float sustain;
  float release;
  float time;
  float value;
  bool active;
};

struct SplashVoice {
    float sampleRate = 48000.0f;
    ADSREnvelope envelope;

    bool active = false;
    float amplitude = 0.0f;

    void trigger(float gain, float attack, float decay, float sustain, float release) {
        envelope.trigger(attack, decay, sustain, release);
        amplitude = gain;
        active = true;
    }

    float render(float dt) {
        if (!active || !envelope.isActive()) {
            active = false;
            return 0.0f;
        }

        envelope.update(dt);

        float env = envelope.getValue();

        // White noise between -1.0 and 1.0
        float noise = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;

        return noise * amplitude * env;
    }
};
