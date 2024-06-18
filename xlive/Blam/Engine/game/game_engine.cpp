#include "stdafx.h"
#include "game_engine.h"

/* public code */

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

void __cdecl game_engine_player_activated(datum player_index)
{
	INVOKE(0x6A29E, 0x69CB6, game_engine_player_activated, player_index);
	return;
}

bool __cdecl game_engine_team_is_enemy(e_game_team a, e_game_team b)
{
	return INVOKE(0x6ADA3, 0x6A5DE, game_engine_team_is_enemy, a, b);
}
