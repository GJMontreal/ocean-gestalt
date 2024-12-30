#include "Configuration.hpp"
#include "Camera.hpp"
#include "jsonloader.hpp"  //rename

#include <memory.h>
#include <fstream>
#include <iostream>

using json = nlohmann::json;
using namespace std;

Configuration::Configuration(const string& fileName) {
  // open the file
  ifstream file(fileName);
  if (file) {
    json data = json::parse(file);
    int i=0;
    for( const auto& element: data.at("waves"))
      {
        auto wave = std::make_shared<Wave>();
        element.get_to(*(wave.get()));
        waves.push_back(std::move(wave));
      }
      auto aCamera = std::make_shared<Camera>();
      json j =  data.at("camera");
      j.get_to(*(aCamera.get()));
      camera = std::move(aCamera);

    cout << "waves " << i << std::endl;

  } else {
    throw std::invalid_argument(string("The file ") + fileName +
                                " doesn't exist");
  }
}
