#include "stdafx.h"
#include "game_engine.h"

namespace game
{
	s_game_engine_globals* s_game_engine_globals::get()
	{
		return *Memory::GetAddress<s_game_engine_globals**>(0x4BF8F8, 0x4EA028);
	}

	int get_game_mode_engine()
	{
		typedef int(__cdecl* get_game_mode_engine_t)();
		auto p_get_game_mode_engine = Memory::GetAddress<get_game_mode_engine_t>(0x5B15E, 0x3CDBE);
		return p_get_game_mode_engine();
	}
}
