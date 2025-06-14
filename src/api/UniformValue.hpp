#pragma once

#include "ApiValue.hpp"
#include <glm/glm.hpp>

using UniformValue = std::variant<
    int,
    float,
    glm::vec2,
    glm::vec3,
    glm::vec4
>;

inline UniformValue apiToUniform(const ApiValue& val) {
  return std::visit([](const auto& v) -> UniformValue {
    using T = std::decay_t<decltype(v)>;
    if constexpr (std::is_same_v<T, std::vector<float>>) {
      switch (v.size()) {
        case 2: return glm::vec2(v[0], v[1]);
        case 3: return glm::vec3(v[0], v[1], v[2]);
        case 4: return glm::vec4(v[0], v[1], v[2], v[3]);
        default: throw std::invalid_argument("Invalid vector size for uniform");
      }
    } else if constexpr (std::is_same_v<T, bool>) {
      return static_cast<int>(v);  // could be float depending on usage
    } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float> ) {
      return v;
    } else {
      static_assert(sizeof(T) == 0, "Unsupported ApiValue type");
    }
  }, val);
}

inline ApiValue uniformToApi(const UniformValue& val) {
  return std::visit([](const auto& v) -> ApiValue {
    using T = std::decay_t<decltype(v)>;
    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float>) {
      return v;
    } else if constexpr (
      std::is_same_v<T, glm::vec2> ||
      std::is_same_v<T, glm::vec3> ||
      std::is_same_v<T, glm::vec4>
    ) {
      std::vector<float> out(v.length());
      for (int i = 0; i < v.length(); ++i) out[i] = v[i];
      return out;
    } else {
      static_assert(sizeof(T) == 0, "Unsupported UniformValue type");
    }
  }, val);
}