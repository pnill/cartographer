#pragma once

#include "real_math.h"
#include "integer_math.h"
#include "real_vector3d.h"
#include "real_matrix4x3.h"

// added these because they properly get optimized by the compiler
template <class T, class U>
inline static T blam_max(T a, U b) 
{
    return a > b ? a : b;
}

template <class T, class U>
inline static T blam_min(T a, U b) 
{
    return a > b ? b : a;
}

// decimal part <0.5, floor
// decimal part >0.5, ceil
inline static int blam_ticks_real_to_integer(float ticks_real) 
{
	return (int)(ticks_real + (ticks_real < 0.0f ? -0.5f : 0.5f));
}