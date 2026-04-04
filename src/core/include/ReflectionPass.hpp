#pragma once
#include <GL/glew.h>

class ReflectionPass {
public:
    ReflectionPass(int width, int height);
    ~ReflectionPass();

    void beginCapture();
    void endCapture(int viewportW, int viewportH);
    GLuint getTextureID() const { return colorTexture; }

private:
    int width;
    int height;
    GLuint fbo = 0;
    GLuint colorTexture = 0;
    GLuint depthRBO = 0;

    void init();
};
