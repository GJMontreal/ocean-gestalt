#include "UniformHandler.hpp"

#include <iostream>
#include <optional>
typedef struct {
  std::string shaderName;
  std::string uniformName;
} Uniform_t;

static std::optional<Uniform_t> splitPath(std::string path){
      size_t pos = path.find('/');
        if (pos != std::string::npos) {
          Uniform_t uniform;
          uniform.shaderName = path.substr(0, pos);
          uniform.uniformName = path.substr(pos + 1);
          return uniform;
        } else {
            return std::nullopt;
          }
    }

    std::optional<std::string> UniformHandler::handleGet(std::string path) {
        //the first part of our path should be the shadername
        size_t pos = path.find('/');
        if (pos != std::string::npos) {
          std::string shaderName = path.substr(0, pos);
          std::string uniform = path.substr(pos + 1);
          return api.getUniform(shaderName, uniform);
        }else{
          return std::nullopt;
        }
    }

    std::optional<std::string> UniformHandler::handlePost(std::string path, std::string value) {
      // auto uniform = splitPath(path);
      if(auto uniform = splitPath(path)){
        // use(*uniform);
        return std::string("0.55"); // until we get this all working
      }else{
        return std::nullopt;
      }
    }

