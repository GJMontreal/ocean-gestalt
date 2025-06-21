#include "Light.hpp"

#include "Configuration.hpp"

Light::Light(std::shared_ptr<Configuration> config) {
  this->position = config->lightPosition;
}
