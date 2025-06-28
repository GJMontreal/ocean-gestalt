#include "TextRenderer.hpp"

#include "asset.hpp"
#include "glError.hpp"
#include "Shader.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>

static const int VERTICES_PER_QUAD = 6;  //two triangles
static const int MAX_CHARACTERS = 10000;
static const size_t VBO_SIZE =  MAX_CHARACTERS * VERTICES_PER_QUAD * sizeof(TextVertex); 

static const int FIRST_CHAR = 32;
static const int NUM_CHARS = 96;

TextRenderer::TextRenderer(const std::string& fontPath, int fontSize) :fontSize(fontSize) {
  packedChars.resize(NUM_CHARS);
  alignedQuads.resize(NUM_CHARS);

  if(!loadFont(fontPath, fontSize)){
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, color));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, texCoord));
    
    glBindVertexArray(0);
    glCheckError(__FILE__, __LINE__);
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
    std::vector<unsigned char> atlas(atlasWidth * atlasHeight, 0);  //our actual bitmap

    stbtt_PackBegin(&packContext, atlas.data(), atlasWidth, atlasHeight, 0, 1,
                    nullptr);
    // stbtt_PackSetOversampling(&packContext, 2, 2); // not sure we need this

    bool ok = stbtt_PackFontRange(&packContext, fontBuffer.data(), 0, int(fontSize),
                                  FIRST_CHAR, NUM_CHARS, packedChars.data());
    // if(!ok){
    //   std::cerr << "packing failed" << std::endl;
    //   return false;
    // }

    stbtt_PackEnd(&packContext);

    
    glGenTextures(1, &atlasTex);
    glBindTexture(GL_TEXTURE_2D, atlasTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, atlas.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);

    for (int i = 0; i < NUM_CHARS; ++i) {
      float unusedX;
      float unusedY;
      stbtt_GetPackedQuad(packedChars.data(), atlasWidth, atlasHeight, i, &unusedX, &unusedY, &alignedQuads[i], 0);

    }
    //for debugging
    stbi_write_png(FONT_DIR "FontAtlas.png", atlasWidth,atlasHeight, 1, atlas.data(),atlasWidth);
    return true;
}

void TextRenderer::setShader(std::shared_ptr<ShaderProgram> shader) {
    this->shader = shader;
    shader->activate();
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, atlasTex);
    GLint loc = glGetUniformLocation(shader->getHandle(), "textAtlas");
    glUniform1i(loc, 0);

    this->shader->deactivate();

    glBindTexture(GL_TEXTURE_2D,0);
}

void TextRenderer::render(){
  render(this->vertices);
}

void TextRenderer::render(std::vector<TextVertex>& vertices){
  // TODO: fix the problems with our textures
    GLint prevProgram, prevVAO, prevTex;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevVAO);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTex);

    shader->activate();
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, atlasTex);
    GLint loc = glGetUniformLocation(shader->getHandle(), "textAtlas");
    glUniform1i(loc, 0);

    // The vertex buffer need to be divided into chunks of size 'VBO_SIZE',
    // Upload them to the VBO and render
    // This is repeated for every divided chunk of the vertex buffer.

    size_t sizeOfVertices = vertices.size() * sizeof(TextVertex);
    uint32_t drawCallCount = (sizeOfVertices / VBO_SIZE) + 1; // aka number of chunks.

    // Render each chunk of vertex data.
    for(int i = 0; i < drawCallCount; i++)
    {
        const TextVertex* data = vertices.data() + i * VBO_SIZE;
        
        uint32_t vertexCount = 
            i == drawCallCount - 1 ? 
            (sizeOfVertices % VBO_SIZE) / sizeof(TextVertex): 
            VBO_SIZE / (sizeof(TextVertex) * 6);

        int uniformLocation = glGetUniformLocation(shader->getHandle(), "projection");
        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(projection));  // not certain about the value for transpose based on our src
  
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 
            0, 
            i == drawCallCount - 1 ? sizeOfVertices % VBO_SIZE : VBO_SIZE,
            data);

        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    shader->deactivate();

    glUseProgram(prevProgram);
    glBindVertexArray(prevVAO);
    glBindTexture(GL_TEXTURE_2D, prevTex);
    glDisable(GL_BLEND);
}

// we'll do multiple drawText, then renderText
void TextRenderer::drawText(const std::string& text, glm::vec3 position, const glm::vec4& color, float size) {

    const auto vertexOrder = std::vector<int>{ 0, 1, 2, 0, 2, 3 }; 
    float pixelScale = 2.0f / screenHeight;

    glm::vec3 localPosition = position;

    for (char ch : text) {
        // Check if the charecter glyph is in the font atlas.
        if(ch >= FIRST_CHAR  && ch <= FIRST_CHAR + NUM_CHARS)  // not sure about this comparison
        {
            if(vertices.size() <= vertexIndex){
                vertices.resize(vertices.size() + VERTICES_PER_QUAD);  //add space for an additional quad
            }

            // Retrive the data that is used to render a glyph of charecter 'ch'
            stbtt_packedchar packedChar = packedChars[ch- FIRST_CHAR];
            stbtt_aligned_quad alignedQuad = alignedQuads[ch - FIRST_CHAR];
            
            // The units of the fields of the above structs are in pixels, 
            // convert them to a unit of what we want be multilplying to pixelScale  
            glm::vec2 glyphSize = 
            {
                (packedChar.x1 - packedChar.x0) * pixelScale * size,
                (packedChar.y1 - packedChar.y0) * pixelScale * size
            };

            glm::vec2 glyphBoundingBoxBottomLeft = 
            {
                localPosition.x + (packedChar.xoff * pixelScale * size),
                localPosition.y - (packedChar.yoff + packedChar.y1 - packedChar.y0) * pixelScale * size
            };

            // The order of vertices of a quad goes top-right, top-left, bottom-left, bottom-right
            auto glyphVertices = std::vector<glm::vec2> 
            {
                { glyphBoundingBoxBottomLeft.x + glyphSize.x, glyphBoundingBoxBottomLeft.y + glyphSize.y },
                { glyphBoundingBoxBottomLeft.x, glyphBoundingBoxBottomLeft.y + glyphSize.y },
                { glyphBoundingBoxBottomLeft.x, glyphBoundingBoxBottomLeft.y },
                { glyphBoundingBoxBottomLeft.x + glyphSize.x, glyphBoundingBoxBottomLeft.y },
            };

            auto glyphTextureCoords = std::vector<glm::vec2> 
            {
                { alignedQuad.s1, alignedQuad.t0 },
                { alignedQuad.s0, alignedQuad.t0 },
                { alignedQuad.s0, alignedQuad.t1 },
                { alignedQuad.s1, alignedQuad.t1 },
            };

            // We need to fill the vertex buffer by 6 vertices to render a quad as we are rendering a quad as 2 triangles
            // The order used is in the 'order' array
            // order = [0, 1, 2, 0, 2, 3] is meant to represent 2 triangles: 
            // one by glyphVertices[0], glyphVertices[1], glyphVertices[2] and one by glyphVertices[0], glyphVertices[2], glyphVertices[3]
            for(int i = 0; i < 6; i++)
            {
                vertices[vertexIndex + i].position = glm::vec3(glyphVertices[vertexOrder[i]], position.z);
                vertices[vertexIndex + i].color = color;
                vertices[vertexIndex + i].texCoord = glyphTextureCoords[vertexOrder[i]];
            }

            vertexIndex += 6;

            // Update the position to render the next glyph specified by packedChar->xadvance.
            localPosition.x += packedChar.xadvance * pixelScale * size;
        }
        else if(ch == '\n'){
          localPosition.y -= fontSize * pixelScale * size;
          localPosition.x = position.x;
        }
    }
}

