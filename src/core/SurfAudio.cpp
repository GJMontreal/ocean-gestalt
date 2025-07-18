#include "SurfAudio.hpp"

#include "GerstnerWave.hpp"

#include <cstdlib>  // for rand()
#include <algorithm> // for std::clamp

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

constexpr float MIN_LEVEL = 0.0f;
constexpr float MAX_LEVEL = 1.0f;
constexpr float HORIZONTAL_OFFSET = 10.0f;
constexpr float GAIN = 1.0f;

SurfAudio::SurfAudio() {
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_f32;
    config.playback.channels = 2;
    config.sampleRate        = sampleRate;
    config.dataCallback      = SurfAudio::dataCallback;
    config.pUserData         = this;

    if (ma_device_init(nullptr, &config, &device) != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize audio device.");
    }

    leftFilter.configure(sampleRate, 600.f, 0.7f);
    rightFilter.configure(sampleRate,1200.0f, 1.0f);
}

SurfAudio::~SurfAudio() {
    stop();
    ma_device_uninit(&device);
}

void SurfAudio::start() {
    ma_device_start(&device);
}

void SurfAudio::stop() {
    ma_device_stop(&device);
}

void SurfAudio::setSteepness(float left, float right) {
    steepnessLeft.store(std::clamp(left, MIN_LEVEL, MAX_LEVEL), std::memory_order_relaxed);
    steepnessRight.store(std::clamp(right, MIN_LEVEL, MAX_LEVEL), std::memory_order_relaxed);
}

void SurfAudio::dataCallback(ma_device* device, void* output, const void*, ma_uint32 frameCount) {

    SurfAudio* self = static_cast<SurfAudio*>(device->pUserData);
    float* out = static_cast<float*>(output);
    float steepnessLeft = self->steepnessLeft.load(std::memory_order_relaxed);
    float steepnessRight = self->steepnessRight.load(std::memory_order_relaxed);

    float dt = 1.0f / self->sampleRate;

    for (ma_uint32 i = 0; i < frameCount; ++i) {
       self->plopTimer -= dt;

       if (self->plopTimer <= 0.0f) {
         auto targetLeft = (rand() % 2 == 0);

         float minInterval = 0.1f;  // lower bound to avoid audio spam
         float maxInterval = targetLeft ? steepnessLeft : steepnessRight;
         maxInterval = 5.0f - maxInterval * 4.0f;
         float intervalRange = maxInterval - minInterval;
         self->plopTimer =
             minInterval + ((float)rand() / RAND_MAX) * intervalRange;

         auto& target = targetLeft ? self->plopsLeft : self->plopsRight;
         for (int p = 0; p < MAX_PLOPS; ++p) {
           if (!target[p].active) {
             float freq = 300.0f + ((float)rand() / RAND_MAX) * 300.0f;
             float gain = 0.2f + ((float)rand() / RAND_MAX) * 0.3f;
             float decay = 40.0f + ((float)rand() / RAND_MAX) * 10.0f;
             target[p].trigger(freq, gain, decay);
             break;
           }
         }
       }

      float noiseLeft = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
      float noiseRight = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;

      float bandNoiseLeft = self->leftFilter.process(noiseLeft);
      float bandNoiseRight = self->rightFilter.process(noiseRight);

      float plopL = 0.0f;
      float plopR = 0.0f;

      for (int p = 0; p < MAX_PLOPS; ++p) {
        plopL += self->plopsLeft[p].process(dt);
        plopR += self->plopsRight[p].process(dt);
      }
          // std::cout << "sample " << i << " " << plopL << std::endl;

      out[i * 2 + 0] = .1f * plopL + bandNoiseLeft * steepnessLeft;    // Left
      out[i * 2 + 1] = .1f * plopR + bandNoiseRight * steepnessRight;  // Right
    }
}

void SurfAudio::generateSurf(const std::vector<std::shared_ptr<Wave>>& waves, glm::vec3 position, float yaw, double time) {
  // generate a point to the left and right at a fixed distance
  glm::vec2 left; 
  glm::vec2 right;
  getOffsetPoints({position.x,position.z}, yaw, left, right);
  
  auto leftSteepness = getSteepness(waves,{left.x,0.0f,left.y}, static_cast<float>(time));
  auto rightSteepness = getSteepness(waves,{right.x,0.0f,right.y}, static_cast<float>(time));

  // get the steepness at that point
  
    setSteepness(leftSteepness * GAIN, rightSteepness * GAIN );
}

void SurfAudio::getOffsetPoints(glm::vec2 position, float yaw, glm::vec2& leftPoint, glm::vec2& rightPoint) {
  glm::vec2 forward = glm::vec2(sin(yaw), cos(yaw));
  // Right vector = 90° rotated forward (in XZ)
  auto right = glm::vec2(forward.y, -forward.x);

  // Left/right offsets
  leftPoint = position - right * HORIZONTAL_OFFSET / 2.0f;
  rightPoint = position + right * HORIZONTAL_OFFSET / 2.0f;
}