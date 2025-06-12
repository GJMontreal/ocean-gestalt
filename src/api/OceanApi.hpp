#pragma once

#include "ApiAdapter.hpp"
#include "UniformInfo.hpp"

#include <optional>

#include <memory>

class OceanGestaltInterface;
class OceanApi : public ApiAdapter {
public:
  OceanApi(OceanGestaltInterface& app);

  void setupShaderNormalInterface();

  void pauseSimulation(bool pause) override;
  void updateSimulation(std::string path, std::string value) override;
  
  virtual std::optional<UniformValue> setUniform(const std::string_view& shaderName,
                                  const std::string_view& uniformName,
                                  UniformValue value) override;
  
  std::optional<std::any> getUniform(std::string shaderName, std::string uniformName) override;

  private:
  OceanGestaltInterface& app;
  std::map<std::string, std::unique_ptr<UniformInspector>> shaderUniforms;
};
