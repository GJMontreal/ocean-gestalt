#include "TextRenderer.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <iostream>
#include <fstream>

TextRenderer::TextRenderer(const std::string& fontPath, int fontSize) {
    std::ifstream ifs(fontPath, std::ios::binary | std::ios::ate);
    if (!ifs) {
        std::cerr << "Failed to load font: " << fontPath << std::endl;
        return;
    }

    auto end = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    fontBuffer.resize(static_cast<size_t>(end));
    ifs.read(reinterpret_cast<char*>(fontBuffer.data()), fontBuffer.size());

    if (!stbtt_InitFont(&fontInfo, fontBuffer.data(), 0)) {
        std::cerr << "Failed to initialize font" << std::endl;
        return;
    }

    scale = stbtt_ScaleForPixelHeight(&fontInfo, static_cast<float>(fontSize));
    stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);
}

void TextRenderer::renderText(const std::string& text, float x, float y) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (char c : text) {
        int ax, lsb;
        stbtt_GetCodepointHMetrics(&fontInfo, c, &ax, &lsb);

        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(&fontInfo, c, scale, scale, &x0, &y0, &x1, &y1);

        int w = x1 - x0;
        int h = y1 - y0;
        std::vector<unsigned char> bitmap(w * h);

        stbtt_MakeCodepointBitmap(&fontInfo, bitmap.data(), w, h, w, scale, scale, c);

        glRasterPos2f(x + x0, y - y0);
        glDrawPixels(w, h, GL_LUMINANCE, GL_UNSIGNED_BYTE, bitmap.data());

        x += ax * scale;
    }
}
