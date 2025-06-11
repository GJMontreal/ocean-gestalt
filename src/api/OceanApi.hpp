#pragma once

#include "ApiAdapter.hpp"
#include <optional>

#include <memory>

class OceanGestaltInterface;
class OceanApi : public ApiAdapter {
public:
  OceanApi(std::shared_ptr<OceanGestaltInterface> app);

  void setupShaderNormalInterface();

  void pauseSimulation(bool pause) override;
  void updateSimulation(std::string path, std::string value) override;

  std::optional<std::string> setUniform(
      const std::string_view& shaderName,
      const std::string_view& uniformName,
      const std::string_view& value) override;

  std::optional<float> setUniform(
      const std::string_view& shaderName,
      const std::string_view& uniformName,
      float value) override;

  std::optional<std::vector<float> > setUniform(
      const std::string_view& shaderName,
      const std::string_view& uniformName,
      std::vector<float> const) override;

  std::optional<bool> setUniform(
      const std::string_view& shaderName,
      const std::string_view& uniformName,
      bool const) override;

  std::optional<std::string> getUniform(std::string shaderName, std::string uniformName) override;

  private:
  std::shared_ptr<OceanGestaltInterface> app;  //this could just be a reference
};
