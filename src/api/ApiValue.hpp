#pragma once

#include <variant>

using ApiValue = std::
    variant<bool, int, float, std::vector<float>>;
