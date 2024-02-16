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

// decimal part <0.5, floor
// decimal part >0.5, ceil
// TODO rework
static auto blam_ticks_real_to_integer(real32 ticks_real) {
	auto tick_val_integer = static_cast<int32>(ticks_real + (ticks_real < 0.0f ? -0.5f : 0.5f));
	return tick_val_integer;
}