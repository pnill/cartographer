#pragma once

#include "real_math.h"

void matrix4x3_interpolate(const real_matrix4x3* previous, const real_matrix4x3* target, float fractional_ticks, real_matrix4x3* out_mat);