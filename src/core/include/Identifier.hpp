#pragma once

#include <string>

struct Identifier {
  virtual std::string getName() const = 0;
  virtual ~Identifier();
};
