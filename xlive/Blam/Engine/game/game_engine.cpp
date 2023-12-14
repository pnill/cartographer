#include "stdafx.h"
#include "game_engine.h"

s_game_engine_globals* game_engine_globals_get(void)
{
	return *Memory::GetAddress<s_game_engine_globals**>(0x4BF8F8, 0x4EA028);
}

bool __cdecl game_engine_get_change_colors(s_player_profile* player_profile, e_game_team team_index, real_rgb_color* change_colors)
{
	return INVOKE(0x6E5C3, 0x6D1BF, game_engine_get_change_colors, player_profile, team_index, change_colors);
}

bool __cdecl game_engine_variant_cleanup(uint16* flags)
{
	return INVOKE(0x5B720, 0x3D380, game_engine_variant_cleanup, flags);
}
