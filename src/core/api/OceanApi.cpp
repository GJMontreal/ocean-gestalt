#include "OceanApi.hpp"

#include "OceanGestalt.hpp"
#include <string>
#include <iostream>

OceanApi::OceanApi(std::shared_ptr<OceanGestalt> app) : app(app) {}

void OceanApi::pauseSimulation(bool pause) {
  std::cout << "pause " << pause << std::endl;
  //we can call into the app to do whatever we need to
  app->toggleSimulation();
}

void OceanApi::updateSimulation(std::string path, std::string value){
  std::cout << "updating " << path << " with " << value << std::endl;
}