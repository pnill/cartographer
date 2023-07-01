#pragma once

struct s_random_math
{
	DWORD seed;
	DWORD random_number;
};

s_random_math* random_math_get_globals();
DWORD random_math_get_seed();
DWORD random_math_get_random_number();
DWORD* random_math_get_random_number_pointer();
void random_math_set_seed(DWORD seed);
void random_math_set_random_number(DWORD random_number);

