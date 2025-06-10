#pragma once

#include <string>
#include <optional>

// Would it be worthwhile to convert this to CRTP?
class ApiAdapter {
public:
    virtual void pauseSimulation(bool pause) = 0;
    virtual void updateSimulation(std::string path, std::string value) = 0;
    virtual void setUniform(std::string path, std::string value) = 0;
    virtual std::optional<std::string> getUniform(std::string shaderName, std::string path) = 0;

    virtual ~ApiAdapter() = default;
};
