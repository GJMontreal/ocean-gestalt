#ifndef __SERIALIZATION_HPP
#define __SERIALIZATION_HPP

#include "Wave.hpp"
#include "Camera.hpp"
#include "Configuration.hpp"

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

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


#endif