#include "FoamRenderPass.hpp"

#include "asset.hpp"

#include <vector>
#include <numeric>
#include <memory.h>

FoamRenderPass::FoamRenderPass(std::shared_ptr<ShaderProgram> shader, int size ) : size(size) , shader(shader){
    initFramebuffer();
    initQuad();
}

FoamRenderPass::~FoamRenderPass() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &foamTexture);
    glDeleteVertexArrays(1, &quadVAO);
}

void FoamRenderPass::initFramebuffer() {
    glGenTextures(1, &foamTexture);
    glBindTexture(GL_TEXTURE_2D, foamTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, size, size, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, foamTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FoamRenderPass::initQuad() {
    // Use a fullscreen quad VAO with 2 triangles
    float verts[] = {
        -1, -1,  1, -1, -1, 1,
        -1,  1,  1, -1,  1, 1
    };
    GLuint vbo;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &vbo);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindVertexArray(0);
}

void FoamRenderPass::render() {
    glViewport(0, 0, size, size);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    shader->activate();

    // the normal map can be bound once - when we create this object, pass in the shader
    // that way our uniform system will function

    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, normalMapTexture);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

float FoamRenderPass::readAverageFoam() {
    std::vector<float> data(size * size);
    glBindTexture(GL_TEXTURE_2D, foamTexture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, data.data());

    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    return sum / data.size();
}
