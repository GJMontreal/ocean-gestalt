#pragma once

#include <string>

class ApiAdapter {
public:
    virtual void pauseSimulation(bool pause) = 0;
    virtual void updateSimulation(std::string path, std::string value) = 0;
    virtual ~ApiAdapter() = default;
};
