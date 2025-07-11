#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <random>

class HeightMapGenerator {
public:
    HeightMapGenerator(int width, int height);

    void generateFBM(float scale = 10.0f, int octaves = 5, float persistence = 0.5f);
    bool writeToFile(const std::string& filename) const;
    bool writeNormalMapToFile(const std::string& filename, float strength = 1.0f) const;

private:
    int width;
    int height;
    std::vector<uint8_t> pixels;

    float hash(float x, float y) const;
    float noise(float x, float y) const;
    float fbm(float x, float y, int octaves, float persistence) const;
};
