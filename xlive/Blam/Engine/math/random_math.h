#pragma once
#include "real_math.h"
#include "random_direction_table.h"

struct s_random_math
{
	uint32 seed;
	uint32 random_number;
};

s_random_math* random_math_get_globals();

uint32 random_math_get_seed();

uint32 random_math_get_random_number();

uint32* get_local_random_seed_address();

void random_math_set_seed(uint32 seed);

void random_math_set_random_number(uint32 random_number);

real32 _real_random_range(uint32* seed, real32 lower_bound, real32 upper_bound);

int16 _random_integer(uint32* seed);

int16 _random_range(uint32* seed, int16 lower_bound, int16 upper_bound);

real_vector3d* _random_direction3d(uint32* seed, const char* type, char* file, int32 line, real_vector3d* direction);