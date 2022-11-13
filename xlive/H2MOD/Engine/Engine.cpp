#include "stdafx.h"

#include "Engine.h"
#include "Blam/Engine/Game/game/game.h"

namespace Engine
{
	e_game_life_cycle get_game_life_cycle()
	{
		typedef e_game_life_cycle(__cdecl get_lobby_state_t)();
		auto p_get_game_life_cycle = Memory::GetAddress<get_lobby_state_t*>(0x1AD660, 0x1A65DD);

		return p_get_game_life_cycle();
	}
}
