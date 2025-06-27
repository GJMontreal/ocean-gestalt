#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "stb_truetype.h"

#include <map>
#include <string>

class ShaderProgram;
struct Glyph {
    float ax;   // advance.x
    float bx;   // bitmap left
    float by;   // bitmap top
    float bw;   // bitmap width
    float bh;   // bitmap height
    float tx0;  // texture x0
    float ty0;  // texture y0
    float tx1;  // texture x1
    float ty1;  // texture y1
};
class TextRenderer {
public:
    TextRenderer(const std::string& fontPath, int fontSize);
    ~TextRenderer() = default;

    void setShader(std::shared_ptr<ShaderProgram> shader) { this->shader = shader;};
    bool loadFont(const std::string& fontPath, int fontSize);
    void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, const glm::mat4& projection);
    
    GLuint getAtlasTexture() const { return atlasTex; }
    const Glyph& getGlyph(char c) const { return glyphs.at(static_cast<unsigned char>(c)); }
    int getFontSize() const { return fontSize; }
private:
    GLuint atlasTex = 0;
    std::shared_ptr<ShaderProgram> shader;

    std::map<unsigned char, Glyph> glyphs;
    stbtt_packedchar packedChars[96];

    GLuint vao = 0;
    GLuint vbo = 0;
    
    stbtt_fontinfo fontInfo;
    std::vector<unsigned char> fontBuffer;

    int fontSize = 16;
    float scale = 1.f;

    int ascent = 0;
    int descent = 0;
    int lineGap = 0;
   
    int screenWidth, screenHeight;
};
