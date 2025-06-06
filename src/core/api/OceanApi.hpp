#pragma once

#include "ApiAdapter.hpp"

#include <memory>

class OceanGestalt;


class OceanApi : public ApiAdapter {
public:
  OceanApi(std::shared_ptr<OceanGestalt> app);
  void pauseSimulation(bool pause) override;
  void updateSimulation(std::string path, std::string value) override;
private:
  std::shared_ptr<OceanGestalt> app;  //this could just be a reference
};
