#include "Configuration.hpp"

#include <fstream>
#include <iostream>
#include "jsonloader.hpp"

using json = nlohmann::json;
using namespace std;

Configuration::Configuration(const string& fileName) {
  // open the file
  ifstream file(fileName);
  if (file) {
    json data = json::parse(file);
    //we should have an array of waves
    int i=0;
    for( const auto& element: data.at("waves"))
      {

        Wave wave;
        element.get_to(wave);
        cout << "wave " << i++ << endl;
      }
  
    cout << "waves " << i << std::endl;

  } else {
    throw std::invalid_argument(string("The file ") + fileName +
                                " doesn't exist");
  }
}