#pragma once
#ifndef _DEBUG
#define BLAM_MATH_INL __forceinline
#else
#define BLAM_MATH_INL 
#endif

// Returns the maximum of the two values
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Returns the minimum of the two values
#define MIN(a, b) ((a) > (b) ? (b) : (a))

// Ensure a value stays within a certain range
#define PIN(v, v_min, v_max) MAX(v_min, MIN(v, v_max))

// decimal part <0.5, floor
// decimal part >0.5, ceil
// TODO rework
int32 blam_ticks_real_to_integer(real32 ticks_real);
