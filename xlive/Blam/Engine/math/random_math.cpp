#include "stdafx.h"

#include "random_math.h"

s_random_math* random_math_get_globals()
{
	return *Memory::GetAddress<s_random_math**>(0x4A8280, 0x4D2500);
}

DWORD random_math_get_seed()
{
	return random_math_get_globals()->seed;
}

DWORD random_math_get_random_number()
{
	return random_math_get_globals()->random_number;
}

DWORD* random_math_get_random_number_pointer()
{
	return &random_math_get_globals()->random_number;
}

void random_math_set_seed(DWORD seed)
{
	random_math_get_globals()->seed = seed;
}

void random_math_set_random_number(DWORD random_number)
{
	random_math_get_globals()->random_number = random_number;
}
