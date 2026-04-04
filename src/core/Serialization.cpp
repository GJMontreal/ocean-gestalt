#include "Serialization.hpp"

#include "Light.hpp"

// Configuration
void to_json(json& j, const Configuration& configuration) {
  j = {{"camera", *(configuration.camera.get())},
       {"light",  configuration.light->getPosition()}};
  j["mesh"]       = {{"size", configuration.meshSize},
                     {"subdivisions", configuration.meshSubdivisions}};
  j["reflection"] = {{"size", configuration.reflectionSize}};
  j["shadow"]     = {{"size", configuration.shadowSize}};

  json models = json::array();
  for (const auto& m : configuration.sceneModels) {
    models.push_back({
      {"name",               m.name},
      {"file",               m.file},
      {"shader",             m.shader},
      {"position",           m.position},
      {"rotation",           m.rotation},
      {"scale",              m.scale.x},
      {"floating",           m.floating},
      {"tethered",           m.tethered},
      {"rightingWeight",     m.rightingWeight},
      {"spinTorqueScale",    m.spinTorqueScale},
      {"angularDamping",     m.angularDamping},
      {"torsionalStiffness", m.torsionalStiffness}
    });
  }
  j["models"] = models;
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
void to_json(json& j, const Camera& camera) {
  j = json{{"position", camera.getPosition()},
           {"up", camera.getMoveDirection().up},
           {"yaw", camera.getYaw()},
           {"pitch", camera.getPitch()},
           {"zoom", camera.getZoom()}};
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
  j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}};
}

void from_json(const json& j, vec4& v) {
  j.at("r").get_to(v.r);
  j.at("g").get_to(v.g);
  j.at("b").get_to(v.b);
  j.at("a").get_to(v.a);
}

void to_json(json& j, const vec4& v) {
  j = json{{"r", v.r}, {"g", v.g}, {"b", v.b}, {"a", v.a}};
}
}  // namespace glm
