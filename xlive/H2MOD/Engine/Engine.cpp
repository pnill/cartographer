#include "stdafx.h"

#include "Engine.h"
#include "Blam/Engine/game/game_globals.h"

namespace Engine
{
	e_game_life_cycle get_game_life_cycle()
	{
		typedef e_game_life_cycle(__cdecl get_lobby_state_t)();
		auto p_get_game_life_cycle = Memory::GetAddress<get_lobby_state_t*>(0x1AD660, 0x1A65DD);

		return p_get_game_life_cycle();
	}

	e_engine_type get_current_engine_type()
	{
		typedef e_engine_type(__cdecl* get_current_engine_type_t)();
		auto p_get_current_engine_type = Memory::GetAddress<get_current_engine_type_t>(0x4980A, 0x42AA7);
		
		return p_get_current_engine_type();
	}

	int get_global_random_seed_address()
	{
		//No need for a typedef just return the same address the function does.
		return Memory::GetAddress(0x4A8280, 0x4D2500);
	}

	int get_game_mode_engine()
	{
		typedef int(__cdecl* get_game_mode_engine_t)();
		auto p_get_game_mode_engine = Memory::GetAddress<get_game_mode_engine_t>(0x5B15E, 0x3CDBE);
		return p_get_game_mode_engine();
	}
}
