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

  std::optional<std::string> setUniform(std::string shaderName,
                                        std::string uniformName,
                                        std::string value) override;

  std::optional<float> setUniform(std::string shaderName,
                                        std::string uniformName,
                                        float value) override;
                                        
  std::optional<std::vector<float> > setUniform(std::string shaderName,
                                        std::string uniformName,
                                        std::vector<float> const) override;

  std::optional<std::string> getUniform(std::string shaderName, std::string uniformName) override;

  private:
  std::shared_ptr<OceanGestaltInterface> app;  //this could just be a reference
};
