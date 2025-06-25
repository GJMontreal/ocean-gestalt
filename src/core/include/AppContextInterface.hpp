#pragma once

#include "ApiValue.hpp"
#include <map>
#include <memory>

class ApiAdapter;
class ShaderProgram;
class WaveInterface;
class Wave;

class AppContextInterface{
  public:
    virtual std::unordered_map<std::string, std::shared_ptr<ShaderProgram> >& getShaders() = 0;
    virtual std::shared_ptr<WaveInterface> getWaveInterface() = 0;
    virtual std::vector<std::shared_ptr<Wave>> getWaves() = 0;
    virtual void setWaveParameter(const std::string& key,const ApiValue& value) = 0;
    virtual void setInitialUniformState(const ApiAdapter& api) = 0;
    virtual void setApi(std::shared_ptr<ApiAdapter> api) = 0;
    virtual ~AppContextInterface() = default;
};
