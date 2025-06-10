#pragma once

#include "ConfigurationInterface.hpp"

class OceanGestaltInterface{
  public:
    virtual ConfigurationInterface& getConfiguration() = 0;
    virtual void doOnReady(const std::function<void()>& callback) = 0;
};
