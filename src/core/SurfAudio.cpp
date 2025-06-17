#include "SurfAudio.hpp"
#include <cstdlib>  // for rand()
#include <algorithm> // for std::clamp

SurfAudio::SurfAudio() {
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_f32;
    config.playback.channels = 2;
    config.sampleRate        = 48000;
    config.dataCallback      = SurfAudio::dataCallback;
    config.pUserData         = this;

    if (ma_device_init(nullptr, &config, &device) != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize audio device.");
    }
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

void SurfAudio::setFoamLevel(float foam) {
    foamLevel.store(std::clamp(foam, 0.0f, 1.0f), std::memory_order_relaxed);
}

void SurfAudio::dataCallback(ma_device* device, void* output, const void*, ma_uint32 frameCount) {
    SurfAudio* self = static_cast<SurfAudio*>(device->pUserData);
    float* out = static_cast<float*>(output);
    float foam = self->foamLevel.load(std::memory_order_relaxed);

    for (ma_uint32 i = 0; i < frameCount; ++i) {
        float noise = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        float sample = noise * foam;

        out[i * 2 + 0] = sample;  // Left
        out[i * 2 + 1] = sample;  // Right
    }
}
