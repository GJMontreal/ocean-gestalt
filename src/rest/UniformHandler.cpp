#include "UniformHandler.hpp"

#include <iostream>
#include <optional>

template <typename T>
SetUniformFunc makeUniformSetter(ApiAdapter& api) {
  return
      [&api](const std::string& shader, const std::string& name,
             const nlohmann::json::value_type& v) -> std::optional<std::any> {
        try {
          auto value = api.setUniform(shader, name, v.get<T>());
          if(value){
            return *value;
          }
          return std::nullopt;
        } catch (...) {
          return std::nullopt;
        }
      };
}

UniformHandler::UniformHandler(ApiAdapter& api) : PathHandler(api) {
  handlers = {
      {[](auto& v) { return v.is_number_float(); },
       makeUniformSetter<float>(api),{"float"}},
      {[](auto& v) {
         return v.is_array();},
       makeUniformSetter<std::vector<float>>(api),{"vector"}},
      {[](auto& v) { return v.is_string(); },
       makeUniformSetter<std::string>(api),{"string"}},
       {[](auto& v) { return v.is_boolean(); },
       makeUniformSetter<bool>(api),{"boolean"}}
      // Add more types as needed
  };
}

std::optional<std::string> UniformHandler::handleGet(const std::string& path) {
  if (auto uniform = splitPath(path)) {
    return api.getUniform(uniform->shaderName, uniform->uniformName);
  } else {
    return std::nullopt;
  }
}

std::optional<std::string> UniformHandler::handlePost(
    const std::string& path,
    nlohmann::json::value_type value) {
  if (auto uniform = splitPath(path)) {

    for (const auto& handler : handlers) {
      if (!handler.match(value)) {
        continue;
      }
      if(auto result = handler.apply(uniform->shaderName, uniform->uniformName, value))
      {
        auto retval = anyToJson(*result);
        if(retval){
          return retval->dump();  // return a stringified JSON
        }
        return std::nullopt;
      }
    }
  }
  return std::nullopt;
}

std::optional<UniformKey> UniformHandler::splitPath(const std::string_view& path) {
  size_t pos = path.find('/');
  if (pos != std::string::npos) {
    UniformKey uniform;
    uniform.shaderName = path.substr(0, pos);
    uniform.uniformName = path.substr(pos + 1);
    return uniform;
  } else {
    return std::nullopt;
  }
}

std::optional<nlohmann::json> UniformHandler::anyToJson(const std::any& a) {
  if (a.type() == typeid(float)) {
    return nlohmann::json(std::any_cast<float>(a));
  } else if (a.type() == typeid(std::string)) {
    return nlohmann::json(std::any_cast<std::string>(a));
  } else if (a.type() == typeid(std::vector<float>)) {
    return nlohmann::json(std::any_cast<std::vector<float>>(a));
  }
  return std::nullopt;
}
