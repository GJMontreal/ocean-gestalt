#pragma once

#include "Wave.hpp"
#include "Camera.hpp"
#include "Configuration.hpp"
#include "ApiValue.hpp"

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include <map>

using json = nlohmann::json;

// Configuration
void to_json(json& j, const Configuration& configuration);

void to_json(json& j, shared_ptr<Wave> wave);

// Wave
void from_json(const json& j, WaveSerialized& wave);
void to_json(json& j, const WaveSerialized& wave); 

// Camera
void from_json(const json& j, Camera& camera);
void to_json(json& j, const Camera& camera);

namespace glm {
void to_json(json& j, const vec2& v);
void from_json(const json& j, vec2& v);

void from_json(const json& j, vec3& v);
void to_json(json& j, const vec3& v);

void from_json(const json& j, vec4& v);
void to_json(json& j, const vec4& v);

}  // namespace glm

// This needs to be in the header for ADL (Argument Dependent Lookup) to work
// Template specializations need to be visible at the point of instantiation
// json j = val resolves to something like
// adl_serializer<T>:to_json(j, val)
namespace nlohmann {
  template <>
  struct adl_serializer<ApiValue> {
    static void to_json(json& j, const ApiValue& value) {
      std::visit([&](const auto& v) {
        j = v;
      }, value);
    }

    static void from_json(const json& j, ApiValue& value) {
      // Try types in order of most restrictive to least
      if (j.is_boolean()) {
        value = j.get<bool>();
      } else if (j.is_number_integer()) {
        value = j.get<int>();
      } else if (j.is_number_float()) {
        value = j.get<float>();
      } else if (j.is_array()) {
        // Optional: ensure all elements are numbers
        std::vector<float> vec;
        for (const auto& el : j) {
          if (!el.is_number()) {
            throw std::runtime_error("Invalid value in vector<float>");
          }
          vec.push_back(el.get<float>());
        }
        value = vec;
      } else {
        throw std::runtime_error("Unsupported JSON type for ApiValue");
      }
    }
  };
}
