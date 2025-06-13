#pragma once

#include "AppContextInterface.hpp"
class OceanGestaltInterface{
  public:
    virtual AppContextInterface& getContext() = 0;
    virtual void pauseSimulation(bool) = 0;
    virtual void doOnReady(const std::function<void()>& callback) = 0;
    virtual void onRender(const std::function<void()>& callback) = 0;
};
