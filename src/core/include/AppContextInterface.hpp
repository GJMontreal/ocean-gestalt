#pragma once

#include "Shader.hpp"

class AppContextInterface{
  public:
    virtual std::unordered_map<std::string, std::shared_ptr<ShaderProgram>>& getShaders() = 0;
    virtual ~AppContextInterface() = default;
};
