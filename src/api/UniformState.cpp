#include "UniformState.hpp"
#include "UniformDispatcher.hpp"
#include "glError.hpp"
#include "Shader.hpp"
#include <GL/glew.h>
#include <iostream>

static const int TIMEOUT = 50;

std::optional<ApiValue> UniformState::setUniform(
    const std::string& shaderName,
    const std::string& uniformName,
    ApiValue value,
    bool block) {
  UniformValue uniformValue = apiToUniform(value);

  PendingUniformUpdate update{shaderName, uniformName, uniformValue};

  std::optional<std::future<bool>> fut;

  if (block) {
    fut.emplace(update.ack.get_future());
  } 
  {
    std::lock_guard lock(updateMutex);
    pendingUpdatesFront[shaderName].push_back(std::move(update));
  }

  if (fut) {
    if (fut->wait_for(std::chrono::milliseconds(TIMEOUT)) ==
        std::future_status::ready) {
          return fut->get() ? std::optional<ApiValue>{value} : std::nullopt;
    }
  }
  return std::nullopt;
}

std::optional<ApiValue> UniformState::getUniform(const std::string& shader, const std::string& uniform) {
  auto it = uniformStates.find(shader + "." + uniform);
  if( it != uniformStates.end()){
    return( uniformToApi(it->second));
  }
  return std::nullopt;
}

void UniformState::seedFromShaders() {
  auto& shaders = context.getShaders();
  for (auto& [shaderName, shader] : shaders) {
    GLuint handle = shader->getHandle();
    shader->activate();

    GLint count = 0;
    glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &count);

    for (GLint i = 0; i < count; ++i) {
      char name[256];
      GLsizei length;
      GLint size;
      GLenum type;
      glGetActiveUniform(handle, i, sizeof(name), &length, &size, &type, name);

      GLint location = glGetUniformLocation(handle, name);
      if (location < 0) continue;

      std::string key = shaderName + "." + name;
      if (uniformStates.count(key)) continue;

      std::optional<UniformValue> val;
      switch (type) {
        case GL_FLOAT: {
          float v;
          glGetUniformfv(handle, location, &v);
          val = v;
          break;
        }
        case GL_INT:
        case GL_BOOL: {
          GLint v;
          glGetUniformiv(handle, location, &v);
          val = static_cast<int>(v);
          break;
        }
        case GL_FLOAT_VEC2: {
          float v[2];
          glGetUniformfv(handle, location, v);
          val = glm::vec2(v[0], v[1]);
          break;
        }
        case GL_FLOAT_VEC3: {
          float v[3];
          glGetUniformfv(handle, location, v);
          val = glm::vec3(v[0], v[1], v[2]);
          break;
        }
        case GL_FLOAT_VEC4: {
          float v[4];
          glGetUniformfv(handle, location, v);
          val = glm::vec4(v[0], v[1], v[2], v[3]);
          break;
        }
        default:
          break;
      }

      if (val) {
        uniformStates[key] = *val;
      }
    }

    shader->deactivate();
  }
}

void UniformState::renderThreadCallback() {
  if (!seeded) {
    seedFromShaders();
    seeded = true;
  }
  {
    std::lock_guard lock(updateMutex);
    std::swap(pendingUpdatesFront, pendingUpdatesBack);
  }

  for (auto& [shaderName, updates] : pendingUpdatesBack) {
    ShaderProgram* shader = nullptr;
    auto shaders = context.getShaders();

    if(auto it = shaders.find(shaderName); it != shaders.end()){
      shader = it->second.get();
    }

    if(!shader){
      std::cerr << "No matching shader for: " << shaderName << std::endl;
      for (auto& u : updates) {
          u.ack.set_value(false);  // indicate failure
      }
      continue;
    }

    // auto _guard = ShaderScope(shader); // TODO: make shader a ptr
    shader->activate();
    for (auto& u : updates) {
      UniformDispatcher<ShaderProgram> dispatcher{shader, u.uniform};
      std::visit(dispatcher, u.value);

#ifdef DEBUG_GL
      if(dispatcher.success && !glCheckError(__FILE__, __LINE__)){
#else
      if(dispatcher.success){
#endif
        uniformStates[u.shader + "." + u.uniform] = u.value;
      }
          u.ack.set_value(dispatcher.success);
    }
    shader->deactivate();
  }
  pendingUpdatesBack.clear();
}

std::unique_ptr<UniformMap> UniformState::dumpUniforms() {
  auto uniformMap = std::make_unique<UniformMap>();
  for(const auto &[key,value] : uniformStates){
    (*uniformMap)[key] = uniformToApi(value);
  }
  return std::move(uniformMap);
}