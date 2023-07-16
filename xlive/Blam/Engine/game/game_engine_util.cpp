#include "stdafx.h"
#include "game_engine_util.h"

#include "Blam/Engine/game/game_globals.h"

int current_game_engine()
{
	typedef int(__cdecl* get_game_mode_engine_t)();
	auto p_get_game_mode_engine = Memory::GetAddress<get_game_mode_engine_t>(0x5B15E, 0x3CDBE);
	return p_get_game_mode_engine();
}

void game_engine_check_for_round_winner()
{
	typedef void(__cdecl game_engine_check_for_round_winner_t)();
	auto p_game_engine_check_for_round_winner = Memory::GetAddress<game_engine_check_for_round_winner_t*>(0x70F49, 0x6FA4A);
	p_game_engine_check_for_round_winner();
}

void game_engine_end_round_with_winner(DWORD player_index, bool go_to_next_round)
{
	typedef void(__cdecl game_engine_end_round_with_winner_t)(DWORD player_index, bool unk_bool);
	auto p_game_engine_end_round_with_winner = Memory::GetAddress<game_engine_end_round_with_winner_t*>(0x70A6F, 0x6F570);
	p_game_engine_end_round_with_winner(player_index, go_to_next_round);
}

bool game_engine_has_teams()
{
	if (current_game_engine())
	{
		return s_game_globals::get_game_variant()->game_engine_flags & 1;
	}

	return false;
}