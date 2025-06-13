#pragma once

#include <variant>
#include <string>
#include <vector>

using UniformValue = std::variant< bool, int, float,
      unsigned int, std::vector<float>, std::string >;
