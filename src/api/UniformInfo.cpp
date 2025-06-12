#include "UniformInfo.hpp"

UniformInspector::UniformInspector(ShaderProgram& shader) : shader(shader) {
  auto uniforms = getActiveUniforms();
  for(auto uniform: uniforms){
    //create a map entry
    this->uniforms.emplace(uniform.name, uniform);
  }
}

std::vector<UniformInfo> UniformInspector::getActiveUniforms() {
  GLint uniformCount = 0;
  auto program = shader.getHandle();
  glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniformCount);

  std::vector<UniformInfo> results;
  char nameBuf[256];

  for (GLint i = 0; i < uniformCount; ++i) {
    GLsizei length = 0;
    GLint size = 0;
    GLenum type = 0;
    glGetActiveUniform(program, i, sizeof(nameBuf), &length, &size, &type,
                       nameBuf);

    GLint location = glGetUniformLocation(program, nameBuf);
    if (location == -1)
      continue;  // optimized out or in UBO

    UniformInfo info;
    info.name = std::string(nameBuf, length);
    info.location = location;
    info.type = type;
    info.size = size;

    info.value = getValue(info);
  }
}

UniformValueType UniformInspector::getValue(const UniformInfo& uniform) {
  UniformValueType value = std::monostate{}; 
  switch (uniform.type) {
      case GL_FLOAT: {
        float v;
        glGetUniformfv(shader.getHandle(), uniform.location, &v);
        value = v;
        break;
      }
      case GL_INT: {
        int v;
        glGetUniformiv(shader.getHandle(), uniform.location, &v);
        value = v;
        break;
      }
      case GL_UNSIGNED_INT: {
        unsigned int v;
        glGetUniformuiv(shader.getHandle(), uniform.location, &v);
        value = v;
        break;
      }
      case GL_FLOAT_VEC4: {
        std::vector<float> v(4);
        glGetUniformfv(shader.getHandle(), uniform.location, v.data());
        value = v;
        break;
      }
      // Extend as needed: GL_FLOAT_VEC2, GL_FLOAT_MAT4, etc.
      default:
        break;
    }
      return value;
}
