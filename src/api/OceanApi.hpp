#pragma once

#include "ApiAdapter.hpp"
#include "UniformState.hpp"
#include "PathHandler.hpp"
#include <optional>


class OceanGestaltInterface;
class OceanApi : public ApiAdapter {
public:
  OceanApi(OceanGestaltInterface& app, UniformState& state);

  void setupShaderNormalInterface();

  void pauseSimulation(bool pause) override;
  void updateSimulation(std::string path, std::string value) override;
  
  std::optional<ApiValue> setValue(const std::string& path, ApiValue& value) override;
  
  std::optional<ApiValue> setUniform(const std::string& shaderName,
                                  const std::string& uniformName,
                                  ApiValue value) override;
  
  std::optional<std::any> getUniform(std::string shaderName, std::string uniformName) override;

  void renderThreadCallback();
  
  protected:
    std::vector<std::string> splitPath(const std::string& input, char delimiter = '.');
  private:
  OceanGestaltInterface& app;
  UniformState& uniformState;

  std::vector<std::unique_ptr<PathHandler>> pathHandlers;
};
