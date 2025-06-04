#pragma once

#include "ApiAdapter.hpp"

#include <memory>

class OceanGestalt;

class OceanApi : public ApiAdapter {
public:
  OceanApi(std::shared_ptr<OceanGestalt> app);
  void pauseSimulation(bool pause) override;
private:
  std::shared_ptr<OceanGestalt> app;
};
