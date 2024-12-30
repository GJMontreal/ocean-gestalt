#include "jsonloader.hpp"
// This could all live in a header...maybe?
// Wave
void to_json(json& j, const Wave& wave) {
  // j = json{{"name", p.name}, {"address", p.address}, {"age", p.age}};
}

void from_json(const json& j, Wave& wave) {
  j.at("amplitude").get_to(wave.amplitude);
  j.at("direction").get_to(wave.direction);
  j.at("wavelength").get_to(wave.wavelength);
  j.at("steepness").get_to(wave.steepness);
}

void from_json(const json& j, Camera& camera){
  j.at("position").get_to(camera.Position);
  j.at("up").get_to(camera.WorldUp);
  j.at("yaw").get_to(camera.Yaw);
  j.at("pitch").get_to(camera.Pitch);
  j.at("zoom").get_to(camera.Zoom);
  camera.updateCameraVectors();
}

namespace glm{
void to_json(json& j, const vec2& v){
}

void from_json(const json& j, vec2& v){
  j.at("x").get_to(v.x);
  j.at("y").get_to(v.y);
}    

void from_json(const json& j, vec3& v){
  j.at("x").get_to(v.x);
  j.at("y").get_to(v.y);
  j.at("z").get_to(v.z);
}

}