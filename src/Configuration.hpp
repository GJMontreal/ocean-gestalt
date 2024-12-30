#ifndef __CONFIGURATION_HPP
#define __CONFIGURATION_HPP

#include "Wave.hpp"
#include "Camera.hpp"

#include <memory.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using namespace std;

class Configuration {
 public:
  explicit Configuration(const std::string& fileName);
  vector<shared_ptr<Wave>> waves;
  shared_ptr<Camera> camera;
};
#endif
