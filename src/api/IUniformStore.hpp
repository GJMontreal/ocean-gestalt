#pragma once

#include "ApiValue.hpp"
#include <optional>
#include <string>
#include <memory>

class IUniformStore {
public:
    virtual std::optional<ApiValue> setUniform(
        const std::string& shaderName,
        const std::string& uniformName,
        ApiValue value,
        bool block = false) = 0;

    virtual std::optional<ApiValue> getUniform(
        const std::string& shaderName,
        const std::string& uniformName) = 0;

    virtual std::unique_ptr<UniformMap> dumpUniforms() = 0;

    virtual ~IUniformStore() = default;
};
