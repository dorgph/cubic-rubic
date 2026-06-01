#pragma once

#include "common.h"

constexpr int CUBE_SIZE = 54;

using State      = std::array<int, CUBE_SIZE>;
using Cycle      = std::vector<int>;
using Cycles     = std::vector<Cycle>;
using Moves      = std::unordered_map<std::string, State>;
using Macros     = std::unordered_map<std::string, std::vector<std::string>>;
