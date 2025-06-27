#include "TextRenderer.hpp"

#include "Shader.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>


TextRenderer::TextRenderer(const std::string& fontPath, int fontSize) {
  if(!loadFont(fontPath, fontSize)){
    throw std::runtime_error("Failed to load font: " + fontPath);
  }
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool TextRenderer::loadFont(const std::string& fontPath, int fontSize){
       std::ifstream fontFile(fontPath, std::ios::binary | std::ios::ate);
    if (!fontFile) return false;

    std::streamsize size = fontFile.tellg();
    fontFile.seekg(0, std::ios::beg);
    std::vector<unsigned char> fontBuffer(size);
    if (!fontFile.read(reinterpret_cast<char*>(fontBuffer.data()), size)) return false;

    stbtt_pack_context packContext;
    const int atlasWidth = 512, atlasHeight = 512;
    std::vector<unsigned char> atlas(atlasWidth * atlasHeight, 0);

    stbtt_PackBegin(&packContext, atlas.data(), atlasWidth, atlasHeight, 0, 1, nullptr);
    stbtt_PackSetOversampling(&packContext, 2, 2);
    stbtt_PackFontRange(&packContext, fontBuffer.data(), 0, fontSize, 32, 96, packedChars);
    stbtt_PackEnd(&packContext);

    glGenTextures(1, &atlasTex);
    glBindTexture(GL_TEXTURE_2D, atlasTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, atlas.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    for (int i = 0; i < 96; ++i) {
        stbtt_packedchar& pc = packedChars[i];
        Glyph g;
        g.ax = pc.xadvance;
        g.bx = pc.xoff;
        g.by = pc.yoff;
        g.bw = pc.x1 - pc.x0;
        g.bh = pc.y1 - pc.y0;
        g.tx0 = pc.x0 / float(atlasWidth);
        g.ty0 = pc.y0 / float(atlasHeight);
        g.tx1 = pc.x1 / float(atlasWidth);
        g.ty1 = pc.y1 / float(atlasHeight);
        glyphs[i + 32] = g;
    }

    return true;
}

void TextRenderer::renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, const glm::mat4& projection) {
    shader->activate();

    glUniformMatrix4fv(glGetUniformLocation(shader->getHandle(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shader->getHandle(), "textColor"), 1, glm::value_ptr(color));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTex);

    glBindVertexArray(vao);
    
    for (char c : text) {
        if (glyphs.find(static_cast<unsigned char>(c)) == glyphs.end()) continue;
        const Glyph& g = glyphs.at(static_cast<unsigned char>(c));

        float xpos = x + g.bx * scale;
        float ypos = y - g.by * scale;
        float w = g.bw * scale;
        float h = g.bh * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h, g.tx0, g.ty1 },
            { xpos,     ypos,     g.tx0, g.ty0 },
            { xpos + w, ypos,     g.tx1, g.ty0 },
            { xpos,     ypos + h, g.tx0, g.ty1 },
            { xpos + w, ypos,     g.tx1, g.ty0 },
            { xpos + w, ypos + h, g.tx1, g.ty1 },
        };

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += g.ax * scale;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    shader->deactivate();
}

