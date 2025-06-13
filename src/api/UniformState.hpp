#pragma once

#include "AppContextInterface.hpp"
#include "UniformValue.hpp"
#include "ApiValue.hpp"

#include <future>
#include <map>
#include <optional>


// TODO: we should change this to use a path for the uniform 
struct PendingUniformUpdate {
    std::string shader;
    std::string uniform;
    UniformValue value;
    std::promise<bool> ack;
};

class UniformState{
  public:
  UniformState(AppContextInterface& context) : context(context){};
  std::optional<UniformValue> setUniform(const std::string& shaderName,
                                  const std::string& uniformName,
                                  UniformValue value);

  void renderThreadCallback();

  private:
  AppContextInterface& context; // is this unowned
  std::mutex updateMutex;
  std::unordered_map<std::string, std::vector<PendingUniformUpdate>> pendingUpdates;
  std::unordered_map<std::string, UniformValue> uniformStates; //we need to store paths in here
};