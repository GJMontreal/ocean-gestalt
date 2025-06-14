#include "UniformHandler.hpp"

#include <optional>

template <typename T>
ApplyFunc makeApplyFunc(){
  return 
  [](const nlohmann::json::value_type& v)->ApiValue{
    return v.get<T>();
  };
}

UniformHandler::UniformHandler(ApiAdapter& api) : PathHandler(api) {
  handlers = {
      {[](auto& v) { return v.is_number_float(); },
       makeApplyFunc<float>(),
       {"float"}},
      {[](auto& v) {
         return v.is_array();},
       makeApplyFunc<std::vector<float> >(),
       {"vector<float>"}},
       {[](auto& v) { return v.is_boolean(); },
       makeApplyFunc<bool>(),
       {"boolean"}}
      // Add more types as needed
  };
}

std::optional<std::string> UniformHandler::handleGet(const std::string& path) {
  if (auto uniform = splitPath(path)) {
    auto retval = api.getUniform(uniform->shaderName, uniform->uniformName);
    if(!retval){
      return std::nullopt;
    }
    return std::string("");
  }
  return std::nullopt;
}

std::optional<std::string> UniformHandler::handlePost(
    const std::string& path,
    nlohmann::json::value_type value) {

  for (const auto& handler : handlers) {
    if (!handler.match(value)) {
      continue;
    }
    ApiValue parsed = handler.apply(value);
    if (auto result = api.setValue(path, parsed)) {
      return std::visit(
          [](auto&& val) -> std::string { return nlohmann::json(val).dump(); },
          *result);
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
