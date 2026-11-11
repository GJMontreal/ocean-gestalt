#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using ApiValue = std::
    variant<bool, int, float, std::vector<float>>;

using UniformMap = std::unordered_map<std:: string, ApiValue>;