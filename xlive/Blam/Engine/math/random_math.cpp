#include "stdafx.h"
#include "random_math.h"

// More info:
// https://www.cec.uchile.cl/cinetica/pcordero/MC_libros/NumericalRecipesinC.pdf
// Page 284 to 285

#define RANDOM_A 1664525
#define RANDOM_C 1013904223
#define RANDOM_NEW_SEED(prev_seed) ((RANDOM_A * (prev_seed) + RANDOM_C))

#define RANDOM_FLONE 1.0f
#define RANDOM_MASK 0xffff // 0xffff

// DIV_BY_MAX_MASK Compiles to 0.000015259022
#define DIV_BY_MAX_MASK_REAL (RANDOM_FLONE / (real32)RANDOM_MASK)

// get the HIGH bits of the seed (between 65535 ... 0)
#define SEED_HIWORD(seed) (seed >> 16)

// integer random
#define INTEGER_RANDOM(seed) ((SEED_HIWORD(seed)))
#define INTEGER_RANDOM_RANGE(seed, lower_bound, delta) ((lower_bound) + (((delta) * INTEGER_RANDOM(seed)) >> 16))

// floating point random
// generates a value between 1 ... 0 <===> (65535 ... 0) / 65535 
// [65535 = (2^16) - 1]
#define REAL_RANDOM(seed) (DIV_BY_MAX_MASK_REAL * SEED_HIWORD(seed))
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
	*seed = RANDOM_NEW_SEED(*seed);
	return REAL_RANDOM_RANGE(*seed, lower_bound, upper_bound - lower_bound);
}

int16 _random_range(uint32* seed, int16 lower_bound, int16 upper_bound)
{
	*seed = RANDOM_NEW_SEED(*seed);
	return INTEGER_RANDOM_RANGE(*seed, lower_bound, upper_bound - lower_bound);
}

int16 _random_integer(uint32* seed)
{
	*seed = RANDOM_NEW_SEED(*seed);
	return INTEGER_RANDOM(*seed);
}

real_vector3d* _random_direction3d(uint32* seed, const char* type, char* file, int32 line, real_vector3d* direction)
{
	int32 index = _random_range(seed, 0, 1026);
	*direction = g_random_direction_table[index];
	return direction;
}
