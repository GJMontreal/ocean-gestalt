#pragma once
#include "miniaudio.h"
#include <atomic>

class SurfAudio {
public:
    SurfAudio();
    ~SurfAudio();

    void start();
    void stop();
    void setFoamLevel(float foam);

private:
    static void dataCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount);

    ma_device device;
    std::atomic<float> foamLevel = 0.0f;
};
