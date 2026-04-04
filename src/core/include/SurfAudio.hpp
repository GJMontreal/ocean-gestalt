#pragma once
#include "miniaudio.h"

#include <glm/glm.hpp>
#include <atomic>
#include <vector>

#include <iostream>

class Wave;

struct Biquad {
    float a0 = 0.0f;
    float a1 = 0.0f;
    float a2 = 0.0f;
    float b1 = 0.0f;
    float b2 = 0.0f;
    float z1 = 0;
    float z2 = 0;

    void configure(float sampleRate, float freq, float Q = 1.0f) {
        float omega = 2.0f * M_PI * freq / sampleRate;
        float alpha = sin(omega) / (2.0f * Q);

        float cos_omega = cos(omega);
        a0 = alpha;
        a1 = 0.0f;
        a2 = -alpha;
        float a_norm = 1.0f + alpha;

        b1 = -2.0f * cos_omega;
        b2 = 1.0f - alpha;

        // normalize
        a0 /= a_norm;
        a1 /= a_norm;
        a2 /= a_norm;
        b1 /= a_norm;
        b2 /= a_norm;
    }

    float process(float input) {
        float output = a0 * input + z1;
        z1 = a1 * input - b1 * output + z2;
        z2 = a2 * input - b2 * output;
        return output;
    }
};

struct Plop {
    float phase = 0.0f;
    float freq;
    float gain;
    float decay;
    float value = 0.0f;
    bool active = false;

    void trigger(float f, float g, float d) {
        phase = 0.0f;
        freq = f;
        gain = g;
        decay = d;
        active = true;
    }

    float process(float dt) {
      if (!active){
        return 0.0f;
      }

      float value = gain * cos(2.0f * M_PI * freq * phase);  // start at max
      phase += dt;
      value *= expf(-decay * phase);
      if (phase > 0.5f)  // kill after 0.5 sec
        active = false;

      return value;
    }
};

class SurfAudio {
public:
    SurfAudio();
    ~SurfAudio();

    void start();
    void stop();
    void setSteepness(float left, float right);
    void generateSurf(const std::vector<std::shared_ptr<Wave>>& waves, glm::vec3 position, float yaw, double time);
private:
    float sampleRate = 48000;
    static void dataCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount);
    void getOffsetPoints(glm::vec2 position, float yaw, glm::vec2& leftPoint, glm::vec2& rightPoint);
    ma_device device;
    std::atomic<float> steepnessLeft = 0.0f;
    std::atomic<float> steepnessRight = 0.0f;

    Biquad leftFilter;
    Biquad rightFilter;
  };
