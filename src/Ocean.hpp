#ifndef OCEAN_HPP
#define OCEAN_HPP

#include "Model.hpp"

class Ocean : public Model{
public:
  Ocean(int meshSize, std::vector<ShaderProgram>shaderPrograms);
};

#endif
