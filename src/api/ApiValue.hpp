#pragma once

#include <variant>
#include <map>

using ApiValue = std::
    variant<bool, int, float, std::vector<float>>;

using UniformMap = std::unordered_map<std:: string, ApiValue>;