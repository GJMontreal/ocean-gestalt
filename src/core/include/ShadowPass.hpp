#pragma once
#include <GL/glew.h>

class ShadowPass {
public:
    ShadowPass(int size);
    ~ShadowPass();

    void beginCapture();
    void endCapture(int viewportW, int viewportH);
    GLuint getDepthTextureID() const { return depthTexture; }

private:
    int size;
    GLuint fbo = 0;
    GLuint depthTexture = 0;

    void init();
};
