#include "UniformHandler.hpp"

#include <iostream>
#include <optional>

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

    bool UniformHandler::handlePost(std::string path, std::string value) {
        std::cout << "Update uniform " << path << " with " << value << std::endl;
        return true;
    }

