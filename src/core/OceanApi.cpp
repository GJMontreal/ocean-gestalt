#include "OceanApi.hpp"

#include "OceanGestalt.hpp"
#include <iostream>

OceanApi::OceanApi(std::shared_ptr<OceanGestalt> app) : app(app) {}

void OceanApi::pauseSimulation(bool pause) {
  std::cout << "pause " << pause << std::endl;
  //we can call into the app to do whatever we need to
}
