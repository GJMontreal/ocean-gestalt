#include "ShadowPass.hpp"

ShadowPass::ShadowPass(int size) : size(size) {
    init();
}

ShadowPass::~ShadowPass() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &depthTexture);
}

void ShadowPass::init() {
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, size, size, 0,
                 GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#ifdef __EMSCRIPTEN__
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
#endif
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
#ifdef __EMSCRIPTEN__
    GLenum drawBuffers[] = { GL_NONE };
    glDrawBuffers(1, drawBuffers);
#else
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
#endif
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowPass::beginCapture() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, size, size);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowPass::endCapture(int viewportW, int viewportH) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewportW, viewportH);
}
