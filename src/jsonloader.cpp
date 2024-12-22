#include "jsonloader.hpp"

void to_json(json& j, const Wave& wave) {
        // j = json{{"name", p.name}, {"address", p.address}, {"age", p.age}};
    }

void from_json(const json& j, Wave& wave) {
        j.at("amplitude").get_to(wave.amplitude);
        j.at("direction").get_to(wave.direction);
        j.at("wavelength").get_to(wave.wavelength);
        j.at("steepness").get_to(wave.steepness);
    }

namespace glm{
void to_json(json& j, const vec2& v){

}

void from_json(const json&j, vec2& v){
  j.at("x").get_to(v.x);
  j.at("y").get_to(v.y);
}    
}