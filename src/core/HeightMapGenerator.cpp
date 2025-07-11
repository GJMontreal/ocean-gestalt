// #define STB_IMAGE_WRITE_IMPLEMENTATION

#include "HeightMapGenerator.hpp"
#include <glm/glm.hpp>
#include "stb_image_write.h"

static constexpr float HASH_VEC_X = 127.1f;
static constexpr float HASH_VEC_Y = 311.7f;

static inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

HeightMapGenerator::HeightMapGenerator(int width, int height)
    : width(width), height(height), pixels(width * height, 0) {}

float HeightMapGenerator::hash(float x, float y) const {
   float dot = x * HASH_VEC_X + y * HASH_VEC_Y;
    float sinVal = std::sin(dot);
    return sinVal - std::floor(sinVal);
}

float HeightMapGenerator::noise(float x, float y) const {
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));
    float xf = x - xi;
    float yf = y - yi;

    float a = hash(xi, yi);
    float b = hash(xi + 1, yi);
    float c = hash(xi, yi + 1);
    float d = hash(xi + 1, yi + 1);

    float u = xf * xf * (3.0f - 2.0f * xf);
    float v = yf * yf * (3.0f - 2.0f * yf);

    return lerp(lerp(a, b, u), lerp(c, d, u), v);
}

float HeightMapGenerator::fbm(float x, float y, int octaves, float persistence) const {
    float sum = 0.0f;
    float amp = 1.0f;
    float freq = 1.0f;
    float maxAmp = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        sum += amp * noise(x * freq, y * freq);
        maxAmp += amp;
        amp *= persistence;
        freq *= 2.0f;
    }

    return sum / maxAmp;
}

void HeightMapGenerator::generateFBM(float scale, int octaves, float persistence) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float u = static_cast<float>(x) / width;
            float v = static_cast<float>(y) / height;
            float value = fbm(u * scale, v * scale, octaves, persistence);
            pixels[y * width + x] = static_cast<uint8_t>(std::clamp(value, 0.0f, 1.0f) * 255);
        }
    }
}

bool HeightMapGenerator::writeToFile(const std::string& filename) const {
    return stbi_write_png(filename.c_str(), width, height, 1, pixels.data(), width) != 0;
}

bool HeightMapGenerator::writeNormalMapToFile(const std::string& filename, float strength) const {
    std::vector<uint8_t> normalPixels(width * height * 3); // RGB

    auto getHeight = [&](int x, int y) -> float {
        x = std::clamp(x, 0, width - 1);
        y = std::clamp(y, 0, height - 1);
        return pixels[y * width + x] / 255.0f;
    };

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float hL = getHeight(x - 1, y);
            float hR = getHeight(x + 1, y);
            float hD = getHeight(x, y - 1);
            float hU = getHeight(x, y + 1);

            float dx = hR - hL;
            float dy = hU - hD;

            glm::vec3 normal = glm::normalize(glm::vec3(-dx * strength, -dy * strength, 1.0f));

            int i = (y * width + x) * 3;
            normalPixels[i + 0] = static_cast<uint8_t>((normal.x * 0.5f + 0.5f) * 255.0f);
            normalPixels[i + 1] = static_cast<uint8_t>((normal.y * 0.5f + 0.5f) * 255.0f);
            normalPixels[i + 2] = static_cast<uint8_t>((normal.z * 0.5f + 0.5f) * 255.0f);
        }
    }

    return stbi_write_png(filename.c_str(), width, height, 3, normalPixels.data(), width * 3) != 0;
}
