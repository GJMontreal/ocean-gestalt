#pragma  once

#include <GL/glew.h>

#include <variant>
#include "Shader.hpp"

// monostate here represents an uninitialized value
using UniformValueType = std::variant<std::monostate, float, int, unsigned int,
               std::vector<float>, std::vector<int>, std::vector<unsigned int>>;

struct UniformInfo {
  std::string name;
  GLint location;
  GLenum type;
  GLint size; // array size
  UniformValueType value;
};

class UniformInspector{
  public:
    UniformInspector(ShaderProgram& program);
    void setUniform(std::string name, float value);
  private:
    ShaderProgram& shader;
    std::map<std::string, UniformInfo> uniforms;
    std::vector<UniformInfo> getActiveUniforms();
    UniformValueType getValue(const UniformInfo& uniform);
};
