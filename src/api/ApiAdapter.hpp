#pragma once

#include <optional>
#include <string>
#include <any>
#include <variant>
#include <typeinfo>

using UniformValue = std::variant< std::monostate, bool, int, float,
      unsigned int, std::vector<float>, std::string >;

class ApiAdapter {
 public:
  virtual void pauseSimulation(bool pause) = 0;
  virtual void updateSimulation(std::string path, std::string value) = 0;

  virtual std::optional<UniformValue> setUniform(const std::string_view& shaderName,
                                  const std::string_view& uniformName,
                                  UniformValue value) = 0;

  virtual std::optional<std::any> getUniform(std::string shaderName,
                                                std::string uniformName) = 0;

  virtual ~ApiAdapter() = default;
};
