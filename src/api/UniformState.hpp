#pragma once

#include "AppContextInterface.hpp"

#include "ApiValue.hpp"
#include "IUniformStore.hpp"
#include "UniformValue.hpp"
#include <future>
#include <optional>

struct PendingUniformUpdate {
    std::string shader;
    std::string uniform;
    UniformValue value;
    std::promise<bool> ack;
};

class UniformState : public IUniformStore {
  public:
  UniformState(AppContextInterface& context) : context(context){};

  std::optional<ApiValue> setUniform(
      const std::string& shaderName,
      const std::string& uniformName,
      ApiValue value,
      bool block = false) override;

  std::optional<ApiValue> getUniform(const std::string& shaderName,
                                     const std::string& uniformName) override;

  std::unique_ptr<UniformMap> dumpUniforms() override;
  
  void renderThreadCallback();

  private:
  void seedFromShaders();

  AppContextInterface& context;
  bool seeded = false;
  std::mutex updateMutex;
  // std::unordered_map<std::string, std::vector<PendingUniformUpdate>> pendingUpdates;
  std::unordered_map<std::string, std::vector<PendingUniformUpdate>> pendingUpdatesFront;
  std::unordered_map<std::string, std::vector<PendingUniformUpdate>> pendingUpdatesBack;
  std::unordered_map<std::string, UniformValue> uniformStates;
  std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> shaderPrograms;
};