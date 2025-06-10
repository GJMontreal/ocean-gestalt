#pragma once

#include "ApiAdapter.hpp"

#include <memory>
// #include <string>

class OceanGestalt;
class OceanApi : public ApiAdapter {
public:
  OceanApi(std::shared_ptr<OceanGestalt> app);
  void pauseSimulation(bool pause) override;
  void updateSimulation(std::string path, std::string value) override;
  void setUniform(std::string path, std::string value) override;
  std::optional<std::string> getUniform(std::string shaderName, std::string path) override;

  private:
  std::shared_ptr<OceanGestalt> app;  //this could just be a reference
};
