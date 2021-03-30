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
		return p_game_is_minimized();
	}
}