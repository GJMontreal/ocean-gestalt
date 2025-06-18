#pragma once

#include "ApiAdapter.hpp"
#include "UniformState.hpp"
#include "PathHandler.hpp"
#include <optional>


class OceanGestaltInterface;
class AppContextInterface;
class OceanApi : public ApiAdapter {
public:
  OceanApi(std::shared_ptr<OceanGestaltInterface> app, UniformState& state);

  void setupShaderNormalInterface();

  void pauseSimulation(bool pause) override;
  void updateSimulation(std::string path, std::string value) override;
  
  std::optional<ApiValue> setValue(const std::string& path, const ApiValue& value) const override;
  std::optional<ApiValue> getValue(const std::string& path) const override;
  
  //we're going to deprecate the following
  std::optional<ApiValue> setUniform(const std::string& shaderName,
                                  const std::string& uniformName,
                                  ApiValue value) override;
  
  std::optional<std::any> getUniform(std::string shaderName, std::string uniformName) override;

  std::unique_ptr<UniformMap> dumpUniforms() override;

  void renderThreadCallback();
  
  protected:
    std::vector<std::string> splitPath(const std::string& input, char delimiter = '.')const;
  private:
  std::weak_ptr<OceanGestaltInterface> app;
  UniformState& uniformState;

  std::vector<std::unique_ptr<PathHandler>> pathHandlers;
};
