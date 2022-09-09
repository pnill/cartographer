#pragma once

#include "real_math.h"
#include "integer_math.h"
#include "real_vector3d.h"
#include "real_matrix4x3.h"

// added these because they properly get optimized by the compiler
template <class T, class U>
inline static auto blam_max(T a, U b) {
    return a > b ? a : b;
}

template <class T, class U>
inline static auto blam_min(T a, U b) {
    return a > b ? b : a;
}