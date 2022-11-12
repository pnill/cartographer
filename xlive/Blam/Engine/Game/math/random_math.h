#pragma once

namespace random_math
{
	struct s_random_math_globals
	{
		DWORD seed;
		DWORD random_number;
	};

	static s_random_math_globals* random_math_globals_ptr;

	void __cdecl random_math_initialize();
	s_random_math_globals* random_math_get_globals();
	DWORD random_math_get_seed();
	DWORD random_math_get_random_number();
	DWORD* random_math_get_random_number_pointer();
	void random_math_set_seed(DWORD seed);
	void random_math_set_random_number(DWORD random_number);
	void ApplyHooks();
}