#pragma once

#include <optional>
#include <string>
#include <any>
#include <variant>
#include "ApiValue.hpp"
#include "UniformValue.hpp"

struct ApiAdapter {
  virtual void pauseSimulation(bool pause) = 0;
  virtual void updateSimulation(std::string path, std::string value) = 0;

  virtual std::optional<ApiValue> setValue(const std::string& path, ApiValue& value) = 0;

  virtual std::optional<UniformValue> setUniform(const std::string& shaderName,
                                  const std::string& uniformName,
                                  UniformValue value) = 0;

  virtual std::optional<std::any> getUniform(std::string shaderName,
                                                std::string uniformName) = 0;

  virtual ~ApiAdapter() = default;
};
