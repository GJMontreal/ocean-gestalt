#include "UniformHandler.hpp"

#include <iostream>
#include <optional>
#include "nlohmann/json.hpp"
#include <vector>

using json = nlohmann::json;
typedef struct {
  std::string shaderName;
  std::string uniformName;
} Uniform_t;

template <typename T>
SetUniformFunc makeUniformSetter(ApiAdapter& api) {
  return [&api](const std::string& shader, const std::string& name,
                const nlohmann::json::value_type& v) {
    return api.setUniform(shader, name, v.get<T>());
  };
}

UniformHandler::UniformHandler(ApiAdapter& api) : PathHandler(api) {
  handlers = {

      {[](auto& v) { return v.is_number_float(); },
       makeUniformSetter<float>(api)},

      {[](auto& v) { return v.is_string(); },
       makeUniformSetter<std::string>(api)},

      {[](auto& v) {
         return v.is_array() &&
                std::all_of(v.begin(), v.end(),
                            [](const auto& el) { return el.is_number(); });
       },
       makeUniformSetter<std::vector<float>>(api)}

      // Add more types as needed
  };
}

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

    std::optional<std::string> UniformHandler::handlePost(
        std::string path,
        nlohmann::json::value_type value) {
      if (auto uniform = splitPath(path)) {
        auto uniformName = uniform->uniformName;
        auto shaderName = uniform->shaderName;

        for (const auto& handler : handlers) {
          if (handler.match(value)) {
            return handler.apply(shaderName, uniformName, value);
          }
        }
      }
      return std::nullopt;
    }
