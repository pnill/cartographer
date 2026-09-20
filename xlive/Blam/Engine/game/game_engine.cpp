#include "stdafx.h"
#include "game_engine.h"

#include "game_time.h"
#include "saved_games/player_profile.h"
#include "simulation/game_interface/simulation_game_action.h"

/* globals */

bool g_game_engine_override_player_speed_update = false;
c_static_flags_no_init<k_maximum_players> g_game_engine_override_player_speed_changed;

/* public code */


void game_engine_apply_patches()
{
	PatchCall(Memory::GetAddress(0x75B22, 0x729FD), game_engine_update_player_movement_traits);
	PatchCall(Memory::GetAddress(0x750F9, 0x721F1), game_engine_update_player_movement_traits);
}

c_game_engine* current_game_engine(void)
{
	return get_game_mode_engines()[game_engine_globals_get()->game_engine_index];
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

c_game_engine* get_slayer_engine()
{

	return get_game_mode_engines()[_game_engine_type_slayer];
}

void __cdecl game_engine_apply_map_patches(void)
{
	INVOKE(0x6EFDB, 0x0, game_engine_apply_map_patches);
	return;
}

bool __cdecl game_engine_get_change_colors(s_player_appearance* player_profile, e_game_team team_index, real_rgb_color* change_colors)
{
	return INVOKE(0x6E5C3, 0x6D1BF, game_engine_get_change_colors, player_profile, team_index, change_colors);
}

bool __cdecl game_engine_variant_cleanup(s_game_variant* variant)
{
	return game_variant_cartographer_settings_validate(variant)
		&& INVOKE(0x5B720, 0x3D380, game_engine_variant_cleanup, variant);
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

void game_engine_cartographer_override_player_speed(datum player_index, real32 player_speed)
{
	player_datum* player = player_get(player_index);
	player->unit_speed = player_speed;
	g_game_engine_override_player_speed_changed.set(DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index), true);
}

void __cdecl game_engine_update_player_movement_traits()
{
	if (g_game_engine_override_player_speed_update)
	{
		c_player_with_unit_iterator it;
		while (it.next())
		{
			if (g_game_engine_override_player_speed_changed.test(it.get_absolute_index()))
			{
				simulation_action_game_engine_player_update(it.get_index(), FLAG(_simulation_action_player_update_bit_2));
				g_game_engine_override_player_speed_changed.set(it.get_absolute_index(), false);
			}
		}
		return;
	}

	c_player_with_unit_iterator it;
	while (it.next())
	{
		datum index = it.get_index();
		player_datum* player = it.get_datum();
		c_game_engine* game_engine = current_game_engine();

		real32 current_speed = player->unit_speed;
		real32 desired_speed = 1.0f;
		
		if (game_engine)
		{
			desired_speed = game_engine->player_speed_multiplier(index);
		}

		real32 new_speed = desired_speed;

		if (current_speed != desired_speed)
		{
			if (current_speed <= desired_speed)
			{
				// climb to target speed 0.15f per update.
				new_speed = (0.15f / (real32)game_tick_rate()) + current_speed;
				
				new_speed = PIN(new_speed, 0, desired_speed);
			}

			player->unit_speed = new_speed;

			simulation_action_game_engine_player_update(index, 2);
		}
	}
}
