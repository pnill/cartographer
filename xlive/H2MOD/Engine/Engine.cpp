#include "Engine.h"

//Engine Calls
namespace Engine
{
	game_life_cycle get_game_life_cycle()
	{
		typedef game_life_cycle(__cdecl get_lobby_state)();
		auto p_get_lobby_state = Memory::GetAddress<get_lobby_state*>(0x1AD660, 0x1A65DD);	//Call to return game life cycle

		return p_get_lobby_state();
	}

	bool __cdecl IsGameMinimized()
	{
		typedef bool(__cdecl* is_game_is_minimized)();
		auto p_game_is_minimized = Memory::GetAddress<is_game_is_minimized>(0x28729);
		
		//Check just in case this gets ran by server
		if (Memory::isDedicatedServer())
			return false;
		
		return p_game_is_minimized();
	}

	e_engine_type get_current_engine_type()
	{
		typedef e_engine_type(p_get_current_engine_type)();
		auto c_get_current_engine_type = Memory::GetAddress<p_get_current_engine_type*>(0x4980A, 0x42AA7);
		
		return c_get_current_engine_type();
	}

	int get_global_random_seed_address()
	{
		//No need for a typedef just return the same address the function does.
		return Memory::GetAddress(0x4A8280, 0x4D2500);
	}
}
