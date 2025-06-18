#pragma once
// #include "Shader.hpp"
#include <map>
#include <memory>

class ApiAdapter;
class ShaderProgram;

class AppContextInterface{
  public:
    virtual std::unordered_map<std::string, std::shared_ptr<ShaderProgram>>& getShaders() = 0;
    virtual void setInitialUniformState(const ApiAdapter& api) = 0;
    virtual void setApi(std::shared_ptr<ApiAdapter> api) = 0;
    virtual ~AppContextInterface() = default;
};
