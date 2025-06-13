#pragma once

#include "ApiAdapter.hpp"
#include "UniformState.hpp"

#include <optional>

class OceanGestaltInterface;
class OceanApi : public ApiAdapter {
public:
  OceanApi(OceanGestaltInterface& app, UniformState& state) :
  app(app), uniformState(state){};

  void setupShaderNormalInterface();

  void pauseSimulation(bool pause) override;
  void updateSimulation(std::string path, std::string value) override;
  
  virtual std::optional<UniformValue> setUniform(const std::string& shaderName,
                                  const std::string& uniformName,
                                  UniformValue value) override;
  
  std::optional<std::any> getUniform(std::string shaderName, std::string uniformName) override;

  void renderThreadCallback();
  
  private:
  OceanGestaltInterface& app;
  UniformState& uniformState;
};
