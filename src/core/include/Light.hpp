#pragma once

#include "Moveable.hpp"
#include "Drawable.hpp"


class Light;

#include "Model.hpp"

#include <glm/glm.hpp>
#include "glm/detail/type_vec.hpp"
using glm::vec3;

class Light: public Moveable{
  public:
    explicit Light(std::shared_ptr<Configuration> configuration);
};

