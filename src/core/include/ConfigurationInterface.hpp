#pragma once

#include "Shader.hpp"
#include <memory>

class ConfigurationInterface{
  public:
    std::shared_ptr<ShaderProgram> meshShader;  
    std::shared_ptr<ShaderProgram> wireframeShader;
    std::shared_ptr<ShaderProgram> normalShader;
};
