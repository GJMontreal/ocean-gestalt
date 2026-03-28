#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "stb_truetype.h"

#include <map>
#include <string>

class ShaderProgram;
struct TextVertex{
  glm::vec3 position;
  glm::vec2 texCoord;
  glm::vec3 tangent = glm::vec3(1.0f);
  glm::vec4 color;
};


//I don't think we use this struct
// struct Glyph {
//     float ax;   // advance.x
//     float bx;   // bitmap left
//     float by;   // bitmap top
//     float bw;   // bitmap width
//     float bh;   // bitmap height
//     float tx0;  // texture x0
//     float ty0;  // texture y0
//     float tx1;  // texture x1
//     float ty1;  // texture y1
// };



class TextRenderer {
public:
    TextRenderer(const std::string& fontPath, int fontSize);
    ~TextRenderer() = default;

    void setShader(std::shared_ptr<ShaderProgram> shader);
    void setProjection(const glm::mat4& proj) { projection = proj;};
    void setScreenHeight(float height) { screenHeight = height;};

    void drawBegin() {
      vertexIndex = 0;
      vertices.clear();
    };
    
    void drawText(const std::string& text, glm::vec3 position, const glm::vec4& color, float size );
    void render();
    
    GLuint getAtlasTexture() const { return atlasTex; }

    float getFontSize() const { return fontSize; }
private:
    GLuint atlasTex = 0;
    std::shared_ptr<ShaderProgram> shader;

    std::vector<stbtt_packedchar> packedChars;
    std::vector<stbtt_aligned_quad> alignedQuads; 

    std::vector<unsigned char> atlas;

    GLuint vao = 0;
    GLuint vbo = 0;

    glm::mat4 projection;

    int vertexIndex = 0;
    std::vector<TextVertex> vertices;

    float fontSize = 16.f;
   
    float screenWidth;
    float screenHeight;  

    bool loadFont(const std::string& fontPath, int fontSize);
    void setupBuffers();

    void render(std::vector<TextVertex>& vertices);
};
