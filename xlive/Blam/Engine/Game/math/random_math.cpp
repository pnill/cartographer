#include "stdafx.h"
#include "Util/Memory.h"
#include "Util/Hooks/Hook.h"

#include "random_math.h"

namespace random_math
{
	typedef void(__cdecl* random_math_initialize_t)();
	random_math_initialize_t p_random_math_initialize;
	typedef s_random_math_globals* (__cdecl* random_math_get_globals_t)();
	random_math_get_globals_t p_random_math_get_globals;
	typedef DWORD(__cdecl* random_math_get_seed_t)();
	random_math_get_seed_t p_random_math_get_seed;
	typedef DWORD(__cdecl* random_math_get_random_number_t)();
	random_math_get_random_number_t p_random_math_get_random_number;
	typedef DWORD* (__cdecl* random_math_get_random_number_pointer_t)();
	random_math_get_random_number_pointer_t p_random_math_get_random_number_pointer;
	typedef void(__cdecl* random_math_set_seed_t)(DWORD seed);
	random_math_set_seed_t p_random_math_set_seed;
	typedef void(__cdecl* random_math_set_random_number_t)(DWORD random_number);
	random_math_set_random_number_t p_random_math_set_random_number;

	void __cdecl random_math_initialize()
	{
		typedef void* (__cdecl game_state_malloc_t)(char* a1, char* a2, unsigned int a3);
		auto p_game_state_malloc = Memory::GetAddress<game_state_malloc_t*>(0x2FF1D);
		typedef DWORD(__cdecl system_milliseconds_t)();
		auto p_system_milliseconds = Memory::GetAddress<system_milliseconds_t*>(0x37E51, 0x2B4CE);
		typedef __time64_t(__cdecl system_seconds_t)();
		auto p_system_seconds = Memory::GetAddress<system_seconds_t*>(0x37E5E, 0x2B4DB);

		DWORD time;
		DWORD miliseconds;

		random_math_globals_ptr = (s_random_math_globals*)p_game_state_malloc("random math", "globals", 8u);
		random_math_globals_ptr->seed = 30888;
		miliseconds = p_system_milliseconds();
		time = (DWORD)p_system_seconds() ^ miliseconds;
		random_math_globals_ptr->random_number = time ^ rand();
	}

	s_random_math_globals* random_math_get_globals()
	{
		return random_math_globals_ptr;
	}

	DWORD random_math_get_seed()
	{
		return random_math_globals_ptr->seed;
	}

	DWORD random_math_get_random_number()
	{
		return random_math_globals_ptr->random_number;
	}

	DWORD* random_math_get_random_number_pointer()
	{
		return &random_math_globals_ptr->random_number;
	}

	void random_math_set_seed(DWORD seed)
	{
		random_math_globals_ptr->seed = seed;
	}

	void random_math_set_random_number(DWORD random_number)
	{
		random_math_globals_ptr->random_number = random_number;
	}

	void ApplyHooks()
	{
		DETOUR_ATTACH(p_random_math_initialize, Memory::GetAddress<random_math_initialize_t>(0x590ED, 0x4E3FC), random_math_initialize);
		DETOUR_ATTACH(p_random_math_get_globals, Memory::GetAddress<random_math_get_globals_t>(0x5908F, 0x4E39E), random_math_get_globals);
		DETOUR_ATTACH(p_random_math_get_seed, Memory::GetAddress<random_math_get_seed_t>(0x59087, 0x4E396), random_math_get_seed);
		DETOUR_ATTACH(p_random_math_get_random_number, Memory::GetAddress<random_math_get_random_number_t>(0x59095, 0x4E3A4), random_math_get_random_number);
		DETOUR_ATTACH(p_random_math_get_random_number_pointer, Memory::GetAddress<random_math_get_random_number_pointer_t>(0x5909E, 0x4E3AD), random_math_get_random_number_pointer);
		DETOUR_ATTACH(p_random_math_set_seed, Memory::GetAddress<random_math_set_seed_t>(0x5912D, 0x4E43C), random_math_set_seed);
		DETOUR_ATTACH(p_random_math_set_random_number, Memory::GetAddress<random_math_set_random_number_t>(0x5913A, 0x4E449), random_math_set_random_number);
	}
}