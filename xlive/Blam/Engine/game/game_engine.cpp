#include "stdafx.h"
#include "game_engine.h"

#include "game_engine_default.h"
#include "game_engine_headhunter.h"
#include "saved_games/game_variant.h"
#include "saved_games/player_profile.h"

/* public code */

void game_engine_apply_patches()
{
	// todo: server offset
	PatchCall(Memory::GetAddress(0x1B1E38, 0x197E62), game_engine_get_simulation_protocol);
	if (Memory::g_memory_is_dedicated_server)
	{
		PatchCall(Memory::GetAddress(0, 0x197DF1), game_engine_get_simulation_protocol);
	}

	game_engine_list_set(_game_engine_type_headhunter, get_global_headhunter_engine_instance());
	//game_engine_list_set(_game_engine_type_headhunter, get_game_mode_engines()[_game_engine_type_koth]);
}

c_game_engine* current_game_engine(void)
{
	s_game_engine_globals* game_engine_globals = game_engine_globals_get();

	ASSERT(game_engine_globals);

	c_game_engine* engine = get_game_mode_engines()[game_engine_globals->game_engine_index];

	return engine;
}

s_game_engine_globals* game_engine_globals_get(void)
{
	return *Memory::GetAddress<s_game_engine_globals**>(0x4BF8F8, 0x4EA028);
}

s_simulation_player_netdebug_data* game_engine_get_netdebug_data(datum player_index)
{
	return &game_engine_globals_get()->netdebug_data[DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index)];
}

c_game_engine** get_game_mode_engines()
{
	return Memory::GetAddress<c_game_engine**>(0x4D8548, 0x4F3CE4);
}

void game_engine_list_set(e_game_engine_type type, c_game_engine* engine)
{
	Memory::GetAddress<c_game_engine**>(0x4D8548, 0x4F3CE4)[type] = engine;
}

e_network_game_simulation_protocol game_engine_get_simulation_protocol(s_game_variant* variant)
{
	e_network_game_simulation_protocol result = _network_game_simulation_protocol_synchronous;

	if (IN_RANGE(variant->variant_game_engine_index, _game_engine_type_ctf, k_game_engine_playable_types))
		result = _network_game_simulation_protocol_distributed;

	return result;
}

c_game_engine* get_slayer_engine()
{
	return get_game_mode_engines()[_game_engine_type_slayer];
}


void __cdecl game_engine_apply_map_patches(void)
{
	INVOKE(0x6EFDB, 0x0, game_engine_apply_map_patches);
	return;
}

bool __cdecl game_engine_get_change_colors(s_player_profile* player_profile, e_game_team team_index, real_rgb_color* change_colors)
{
	return INVOKE(0x6E5C3, 0x6D1BF, game_engine_get_change_colors, player_profile, team_index, change_colors);
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

void __cdecl game_engine_update_after_game(void)
{
	INVOKE(0x7156A, 0x7006B, game_engine_update_after_game);
	return;
}

void __cdecl game_engine_update(void)
{
	INVOKE(0x7590F, 0x727EA, game_engine_update);
	return;
}

void __cdecl game_engine_render(void)
{
	INVOKE(0x6A60F, 0x0, game_engine_render);
	return;
}

void __cdecl game_engine_adjust_score(int32 player_index, int32 amount)
{
	INVOKE(0x70E13, 0x6F914, game_engine_adjust_score, player_index, amount);
}