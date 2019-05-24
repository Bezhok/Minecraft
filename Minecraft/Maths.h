#pragma once

#include <cmath>

static const float PI = std::atanf(1) * 4.f;



size_t calculate_3D_hash(int x, int y, int z);


size_t calculate_2D_hash(int x, int y);