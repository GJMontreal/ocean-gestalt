#pragma once

#include <optional>
#include <string>

// Would it be worthwhile to convert this to CRTP?
class ApiAdapter {
 public:
  virtual void pauseSimulation(bool pause) = 0;
  virtual void updateSimulation(std::string path, std::string value) = 0;

  virtual std::optional<std::string> setUniform(
      const std::string_view& shaderName,
      const std::string_view& uniformName,
      const std::string_view& value) = 0;

  virtual std::optional<float> setUniform(
      const std::string_view& shaderName,
      const std::string_view& uniformName,
      float value) = 0;

  virtual std::optional<std::vector<float> > setUniform(
      const std::string_view& shaderName,
      const std::string_view& uniformName,
      std::vector<float> const) = 0;

  virtual std::optional<bool> setUniform(
      const std::string_view& shaderName,
      const std::string_view& uniformName,
      bool const) = 0;

  virtual std::optional<std::string> getUniform(std::string shaderName,
                                                std::string uniformName) = 0;

  virtual ~ApiAdapter() = default;
};
