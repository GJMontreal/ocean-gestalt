#pragma once
#include <glm/glm.hpp>
#include "Shader.hpp"  // Your existing shader class

class FoamRenderPass {
public:
    FoamRenderPass(std::shared_ptr<ShaderProgram> shader, int size = 32);
    ~FoamRenderPass();

    void render();
    float readAverageFoam();

private:
    int size;
    GLuint fbo = 0;
    GLuint foamTexture = 0;
    GLuint quadVAO = 0;
    std::shared_ptr<ShaderProgram> shader;

    void initFramebuffer();
    void initQuad();
};
