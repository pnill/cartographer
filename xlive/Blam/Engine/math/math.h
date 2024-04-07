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

#define CLAMP_LOWER(x, low, high) ((x) >= (high) - (low) ? (x) - (high) : (low))

#define CLAMP_UPPER(x, low, high) ((x) <= (high) - (low) ? (x) + (low) : (high))

// decimal part <0.5, floor
// decimal part >0.5, ceil
// TODO rework
static auto blam_ticks_real_to_integer(real32 ticks_real) {
	auto tick_val_integer = static_cast<int32>(ticks_real + (ticks_real < 0.0f ? -0.5f : 0.5f));
	return tick_val_integer;
}