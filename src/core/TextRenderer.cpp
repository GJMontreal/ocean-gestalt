// adapted from
// https://github.com/shreyaspranav/stb-truetype-example/
// 

#include "TextRenderer.hpp"

#include "Shader.hpp"
#include "asset.hpp"
#include "glError.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>



static const int VERTICES_PER_QUAD = 6;  // two triangles
static const int MAX_CHARACTERS = 10000;
static const unsigned long VBO_SIZE =
    MAX_CHARACTERS * VERTICES_PER_QUAD * sizeof(TextVertex);

static const int FIRST_CHAR = 32;
static const int NUM_CHARS = 95;

const int ATLAS_WIDTH = 512;
const int ATLAS_HEIGHT = 512;

static bool isRenderable(char ch){
  return ch >= FIRST_CHAR &&
        ch < FIRST_CHAR + NUM_CHARS;
}

TextRenderer::TextRenderer(const std::string& fontPath, int fontSize)
    : fontSize(fontSize) {
  packedChars.resize(NUM_CHARS);
  alignedQuads.resize(NUM_CHARS);

  if (!loadFont(fontPath, fontSize)) {
    throw std::runtime_error("Failed to load font: " + fontPath);
  }
  setupBuffers();
}

void TextRenderer::setupBuffers() {
  // Setting up the VAO and VBO: -----------------------
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, VBO_SIZE, nullptr, GL_DYNAMIC_DRAW);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextVertex),
                        (void*)offsetof(TextVertex, position));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex),
                        (void*)offsetof(TextVertex, texCoord));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex),
                        (void*)offsetof(TextVertex, tangent));
  
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex),
                        (void*)offsetof(TextVertex, color));



  glBindVertexArray(0);
  glCheckError(__FILE__, __LINE__);
}

bool TextRenderer::loadFont(const std::string& fontPath, int fontSize) {
  std::ifstream fontFile(fontPath, std::ios::binary | std::ios::ate);
  if (!fontFile)
    return false;

  std::streamsize size = fontFile.tellg();
  fontFile.seekg(0, std::ios::beg);
  std::vector<unsigned char> fontBuffer(size);
  if (!fontFile.read(reinterpret_cast<char*>(fontBuffer.data()), size))
    return false;

  stbtt_pack_context packContext;

  atlas.resize(ATLAS_WIDTH * ATLAS_HEIGHT, 0);
  stbtt_PackBegin(&packContext, atlas.data(), ATLAS_WIDTH, ATLAS_HEIGHT, 0, 1,
                  nullptr);

  
  bool ok = stbtt_PackFontRange(&packContext, fontBuffer.data(), 0, int(fontSize),
                          FIRST_CHAR, NUM_CHARS, packedChars.data());
  if (! ok) {
    for (int i = 0; i < NUM_CHARS; ++i) {
      const auto& pc = packedChars[i];
      if (pc.x0 == 0 && pc.x1 == 0 && pc.y0 == 0 && pc.y1 == 0) {
        std::cout << "Glyph " << (FIRST_CHAR + i) << " failed to pack.\n";
      }
    }
    return false;
  }

  stbtt_PackEnd(&packContext);

  for (int i = 0; i < NUM_CHARS; ++i) {
    float unusedX;
    float unusedY;
    stbtt_GetPackedQuad(packedChars.data(), ATLAS_WIDTH, ATLAS_HEIGHT, i,
                        &unusedX, &unusedY, &alignedQuads[i], 0);
  }
#ifdef DEBUG_GL
  stbi_write_png(FONT_DIR "FontAtlas.png", ATLAS_WIDTH, ATLAS_HEIGHT, 1,
                 atlas.data(), ATLAS_WIDTH);
#endif

  return true;
}

void TextRenderer::setShader(std::shared_ptr<ShaderProgram> aShader) {
  assert(aShader);
  this->shader = aShader;
  auto _guard = ShaderScope(shader);
  glGenTextures(1, &atlasTex);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, atlasTex);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  std::vector<uint8_t> rgbAtlas;
  rgbAtlas.reserve(atlas.size() * 3);

  for (uint8_t gray : atlas) {
    rgbAtlas.push_back(gray);  // R
    rgbAtlas.push_back(gray);  // G
    rgbAtlas.push_back(gray);  // B
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ATLAS_WIDTH, ATLAS_HEIGHT, 0, GL_RGB,
               GL_UNSIGNED_BYTE, rgbAtlas.data());
#ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif
}

void TextRenderer::render() {
  render(this->vertices);
}

void TextRenderer::render(std::vector<TextVertex>& vertices) {
  auto _guard = ShaderScope(shader);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, atlasTex);
  GLint loc = glGetUniformLocation(shader->getHandle(), "textAtlas");
  glUniform1i(loc, 0);

  int uniformLocation =
        glGetUniformLocation(shader->getHandle(), "projection");
    glUniformMatrix4fv(
        uniformLocation, 1, GL_FALSE,
        glm::value_ptr(projection));  

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    const size_t maxVerticesPerBatch = VBO_SIZE / sizeof(TextVertex);

    // Render each chunk of vertex data.
    for (size_t offset = 0; offset < vertices.size();
         offset += maxVerticesPerBatch) {
      size_t batchVertexCount =
          std::min(maxVerticesPerBatch, vertices.size() - offset);
      const TextVertex* data = vertices.data() + offset;

      glBufferSubData(GL_ARRAY_BUFFER, 0, batchVertexCount * sizeof(TextVertex),
                      data);
      glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(batchVertexCount));
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

#ifdef DEBUG_GL
  glCheckError(__FILE__, __LINE__);
#endif
}

// we'll do multiple drawText, then renderText
void TextRenderer::drawText(const std::string& text,
                            glm::vec3 position,
                            const glm::vec4& color,
                            float size) {
  const auto vertexOrder = std::vector<int>{0, 1, 2, 0, 2, 3};
  float pixelScale = 2.0f / screenHeight;

  glm::vec3 localPosition = position;
  vertices.resize(vertices.size() + text.length() * VERTICES_PER_QUAD);
  for (char ch : text) {
    // Check if the charecter glyph is in the font atlas.
    if (isRenderable(ch))
    {
      // Retrieve the data that is used to render a glyph of charecter 'ch'
      stbtt_packedchar packedChar = packedChars[ch - FIRST_CHAR];
      stbtt_aligned_quad alignedQuad = alignedQuads[ch - FIRST_CHAR];

      // The units of the fields of the above structs are in pixels,
      // convert them to a unit of what we want be multilplying to pixelScale
      glm::vec2 glyphSize = {
          (packedChar.x1 - packedChar.x0) * pixelScale * size,
          (packedChar.y1 - packedChar.y0) * pixelScale * size};

      glm::vec2 glyphBoundingBoxBottomLeft = {
          localPosition.x + (packedChar.xoff * pixelScale * size),
          localPosition.y - (packedChar.yoff + packedChar.y1 - packedChar.y0) *
                                pixelScale * size};

      // The order of vertices of a quad goes top-right, top-left, bottom-left,
      // bottom-right
      auto glyphVertices = std::vector<glm::vec2>{
          {glyphBoundingBoxBottomLeft.x + glyphSize.x,
           glyphBoundingBoxBottomLeft.y + glyphSize.y},
          {glyphBoundingBoxBottomLeft.x,
           glyphBoundingBoxBottomLeft.y + glyphSize.y},
          {glyphBoundingBoxBottomLeft.x, glyphBoundingBoxBottomLeft.y},
          {glyphBoundingBoxBottomLeft.x + glyphSize.x,
           glyphBoundingBoxBottomLeft.y},
      };

      auto glyphTextureCoords = std::vector<glm::vec2>{
          {alignedQuad.s1, alignedQuad.t0},
          {alignedQuad.s0, alignedQuad.t0},
          {alignedQuad.s0, alignedQuad.t1},
          {alignedQuad.s1, alignedQuad.t1},
      };

      // We need to fill the vertex buffer by 6 vertices to render a quad as we
      // are rendering a quad as 2 triangles The order used is in the 'order'
      // array order = [0, 1, 2, 0, 2, 3] is meant to represent 2 triangles: one
      // by glyphVertices[0], glyphVertices[1], glyphVertices[2] and one by
      // glyphVertices[0], glyphVertices[2], glyphVertices[3]
      for (int i = 0; i < 6; i++) {
        vertices[vertexIndex + i].position =
            glm::vec3(glyphVertices[vertexOrder[i]], position.z);
        vertices[vertexIndex + i].color = color;
        vertices[vertexIndex + i].texCoord = glyphTextureCoords[vertexOrder[i]];
      }

      vertexIndex += 6;

      localPosition.x += packedChar.xadvance * pixelScale * size;
    } else if (ch == '\n') {
      localPosition.y -= fontSize * pixelScale * size;
      localPosition.x = position.x;
    }
  }
}
