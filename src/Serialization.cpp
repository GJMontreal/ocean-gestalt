#include "Serialization.hpp"
#include <iostream>

// Configuration
void to_json(json& j, const Configuration& configuration) {
  j = json{{"camera",
            *(configuration.camera.get())},
            {"waves", configuration.waves},
            {"light",configuration.lightPosition}};
}

void to_json(json& j, shared_ptr<Wave> p) {
  auto wave = (*p.get());
  j = json{{"amplitude", wave.amplitude},
           {"wavelength", wave.wavelength},
           {"heading", wave.heading},
           {"steepness", wave.steepness}};
}

// Wave
void from_json(const json& j, WaveSerialized& wave) {
  j.at("amplitude").get_to(wave.amplitude);
  j.at("heading").get_to(wave.heading);
  j.at("wavelength").get_to(wave.wavelength);
  j.at("steepness").get_to(wave.steepness);
}

void to_json(json& j, const WaveSerialized& wave) {
  j = json{{"amplitude", wave.amplitude},
           {"wavelength", wave.wavelength},
           {"heading", wave.heading},
           {"steepness", wave.steepness}};
}

// Camera
void from_json(const json& j, Camera& camera) {
  j.at("position").get_to(camera.Position);
  j.at("up").get_to(camera.WorldUp);
  j.at("yaw").get_to(camera.Yaw);
  j.at("pitch").get_to(camera.Pitch);
  j.at("zoom").get_to(camera.Zoom);
  camera.updateCameraVectors();
}

void to_json(json& j, const Camera& camera) {
  j = json{{"position", camera.Position},
           {"up", camera.WorldUp},
           {"yaw", camera.Yaw},
           {"pitch", camera.Pitch},
           {"zoom", camera.Zoom}};
}

namespace glm {
void from_json(const json& j, vec2& v) {
  j.at("x").get_to(v.x);
  j.at("y").get_to(v.y);
}

void to_json(json& j, const vec2& v) {
  j = json{{"x", v.x}, {"y", v.y}};
}

void from_json(const json& j, vec3& v) {
  j.at("x").get_to(v.x);
  j.at("y").get_to(v.y);
  j.at("z").get_to(v.z);
}

void to_json(json& j, const vec3& v) {
  j = json{{"x", v.x}, {"y", v.y},{"z",v.z}};
}

}  // namespace glm