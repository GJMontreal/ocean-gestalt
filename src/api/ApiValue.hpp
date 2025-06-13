#pragma once

#include <variant>

using ApiValue = std::
    variant<bool, int, float, unsigned int, std::vector<float>, std::string>;
