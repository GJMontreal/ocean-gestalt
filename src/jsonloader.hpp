#ifndef __JSONLOADER_HPP
#define __JSONLOADER_HPP

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include "Wave.hpp"

using json = nlohmann::json;

// Wave
void to_json(json& j, const Wave& wave);
void from_json(const json& j, Wave& wave);

namespace glm {
void to_json(json& j, const vec2& v);
void from_json(const json& j, vec2& v);
}  // namespace glm
#endif