#pragma once

#include <string>
#include <GL/glew.h>

#include "stb_truetype.h"

#include <vector>

class TextRenderer {
public:
    TextRenderer(const std::string& fontPath, int fontSize);
    ~TextRenderer() = default;

    void renderText(const std::string& text, float x, float y);
    // void setScreenSize(int width, int height);

private:
    GLuint fontTexture;

    stbtt_fontinfo fontInfo;
    std::vector<unsigned char> fontBuffer;

    int fontSize = 16;
    float scale = 1.f;

    int ascent = 0;
    int descent = 0;
    int lineGap = 0;



    stbtt_bakedchar cdata[96];
    int screenWidth, screenHeight;
};
