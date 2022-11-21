#pragma once

#include "real_math.h"
#include "integer_math.h"
#include "Blam/Engine/Game/math/matrix_math.h"

// added these because they properly get optimized by the compiler
template <class T, class U>
inline static auto blam_max(T a, U b) {
    return a > b ? a : b;
}

template <class T, class U>
inline static auto blam_min(T a, U b) {
    return a > b ? b : a;
}

// decimal part <0.5, floor
// decimal part >0.5, ceil
inline static auto blam_ticks_real_to_integer(float ticks_real) {
	auto tick_val_integer = static_cast<int>(ticks_real + (ticks_real < 0.0f ? -0.5f : 0.5f));
	return tick_val_integer;
}