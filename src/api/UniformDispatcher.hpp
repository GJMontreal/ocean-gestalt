#pragma once

#include <string>
#include <type_traits>
#include <glm/glm.hpp>

// Forward declaration
class ShaderProgram;

template <typename T, typename ShaderT>
struct UniformTraits;

template <typename ShaderT>
struct UniformTraits<int, ShaderT> {
  static void apply(ShaderT* shader, const std::string& name, int value) {
    shader->setUniform(name, value);
  }
};

template <typename ShaderT>
struct UniformTraits<float, ShaderT> {
  static void apply(ShaderT* shader, const std::string& name, float value) {
    shader->setUniform(name, value);
  }
};

template <typename ShaderT>
struct UniformTraits<glm::vec2, ShaderT> {
  static void apply(ShaderT* shader, const std::string& name, const glm::vec2& value) {
    shader->setUniform(name, value);
  }
};

template <typename ShaderT>
struct UniformTraits<glm::vec3, ShaderT> {
  static void apply(ShaderT* shader, const std::string& name, const glm::vec3& value) {
    shader->setUniform(name, value);
  }
};

template <typename ShaderT>
struct UniformTraits<glm::vec4, ShaderT> {
  static void apply(ShaderT* shader, const std::string& name, const glm::vec4& value) {
    shader->setUniform(name, value);
  }
};

template <typename ShaderT>
struct UniformDispatcher {
  ShaderT* shader;
  const std::string& name;
  bool success = false;

  template <typename T>
  void operator()(const T& val) {
    using ApplyFn = void(*)(ShaderT*, const std::string&, T);
    if constexpr (std::is_invocable<ApplyFn, ShaderT*, const std::string&, T>::value) {
      UniformTraits<T, ShaderT>::apply(shader, name, val);
      success = true;
    } else {
      static_assert(sizeof(T) == 0, "Unsupported uniform type");
    }
  }
};
