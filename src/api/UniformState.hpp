#pragma once

#include "AppContextInterface.hpp"

#include "ApiValue.hpp"
#include "UniformValue.hpp"
#include <future>
#include <optional>

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

  std::optional<ApiValue> getUniform(const std::string& shaderName,
                                     const std::string& uniformName);

  void renderThreadCallback();

  private:
  AppContextInterface& context;
  std::mutex updateMutex;
  std::unordered_map<std::string, std::vector<PendingUniformUpdate>> pendingUpdates;
  std::unordered_map<std::string, UniformValue> uniformStates;
  std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> shaderPrograms;
};