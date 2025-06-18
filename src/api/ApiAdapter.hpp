#pragma once

#include <optional>
#include <string>
#include <any>

#include "ApiValue.hpp"

struct ApiAdapter {
  virtual void pauseSimulation(bool pause) = 0;
  virtual void updateSimulation(std::string path, std::string value) = 0;

  virtual std::optional<ApiValue> setValue(const std::string& path,const ApiValue& value) const = 0;
  virtual std::optional<ApiValue> getValue(const std::string& path) const = 0;
  
  virtual std::optional<ApiValue> setUniform(const std::string& shaderName,
                                  const std::string& uniformName,
                                  ApiValue value) = 0;

  virtual std::optional<std::any> getUniform(std::string shaderName,
                                                std::string uniformName) = 0;

  virtual std::unique_ptr<UniformMap> dumpUniforms() = 0;
  // virtual void loadUniforms(const std::string&) = 0;

  virtual ~ApiAdapter() = default;
};
