#pragma once

class ApiAdapter {
public:
    virtual void pauseSimulation(bool pause) = 0;
    virtual ~ApiAdapter() = default;
};
