#ifndef __LIGHT_HPP
#define __LIGHT_HPP

#include "Moveable.hpp"

#include <glm/glm.hpp>
#include "glm/detail/type_vec.hpp"

using glm::vec3;

class Light: public Moveable{
  public:
    explicit Light(vec3 aPosition);
    void ProcessKeyboard(Movement direction, float deltaTime) override;
    vec3 position;
};

#endif
