#pragma once

#include "AppContextInterface.hpp"

#include "ApiValue.hpp"
#include "UniformValue.hpp"
#include <future>
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
  std::optional<ApiValue> setUniform(const std::string& shaderName,
                                  const std::string& uniformName,
                                  ApiValue value);

  void renderThreadCallback();

  private:
  AppContextInterface& context; // is this unowned
  std::mutex updateMutex;
  std::unordered_map<std::string, std::vector<PendingUniformUpdate>> pendingUpdates;
  std::unordered_map<std::string, UniformValue> uniformStates; //we need to store paths in here
};