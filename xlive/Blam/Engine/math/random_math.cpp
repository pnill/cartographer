#include "stdafx.h"
#include "random_math.h"

// More info:
// https://www.cec.uchile.cl/cinetica/pcordero/MC_libros/NumericalRecipesinC.pdf
// Page 284 to 285

#define RANDOM_A 1664525
#define RANDOM_C 1013904223

#define RANDOM_FLONE 1.0f
#define RANDOM_MASK 0xffff

// RANDOM_MULTIPLIER Compiles to 0.000015259022
#define RANDOM_MULTIPLIER RANDOM_FLONE / (real32)RANDOM_MASK

#define RANDOM(seed) (((seed) = RANDOM_A * (seed) + RANDOM_C) >> 16)
#define REAL_RANDOM(seed) (RANDOM(seed) * RANDOM_MULTIPLIER)

#define RANDOM_RANGE(seed, lower_bound, delta) ((lower_bound) + (((delta) * RANDOM(seed)) >> 16))
#define REAL_RANDOM_RANGE(seed, lower_bound, delta) ((lower_bound) + (delta) * REAL_RANDOM(seed))

s_random_math* random_math_get_globals()
{
	return *Memory::GetAddress<s_random_math**>(0x4A8280, 0x4D2500);
}

uint32 random_math_get_seed()
{
	return random_math_get_globals()->seed;
}

uint32 random_math_get_random_number()
{
	return random_math_get_globals()->random_number;
}

uint32* get_local_random_seed_address()
{
	return &random_math_get_globals()->random_number;
}

void random_math_set_seed(uint32 seed)
{
	random_math_get_globals()->seed = seed;
}

void random_math_set_random_number(uint32 random_number)
{
	random_math_get_globals()->random_number = random_number;
}

real32 _real_random_range(uint32* seed, real32 lower_bound, real32 upper_bound)
{
	return REAL_RANDOM_RANGE(*seed, lower_bound, upper_bound - lower_bound);
}

int16 _random_range(uint32* seed, int16 lower_bound, int16 upper_bound)
{
	return RANDOM_RANGE(*seed, lower_bound, upper_bound - lower_bound);
}

real_vector3d* _random_direction3d(uint32* seed, const char* type, char* file, int32 line, real_vector3d* direction)
{
	int16 index = _random_range(seed, 0, 1026);
	*direction = g_random_direction_table[index];
	return direction;
}
