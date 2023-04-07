#include "stdafx.h"
#include "game_engine_util.h"

int current_game_engine()
{
	typedef int(__cdecl* get_game_mode_engine_t)();
	auto p_get_game_mode_engine = Memory::GetAddress<get_game_mode_engine_t>(0x5B15E, 0x3CDBE);
	return p_get_game_mode_engine();
}

