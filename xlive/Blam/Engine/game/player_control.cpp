#include "stdafx.h"
#include "player_control.h"

#include "game.h"
#include "players.h"

#include "cache/cache_files.h"
#include "camera/observer.h"
#include "game/game_time.h"
#include "main/interpolator.h"
#include "saved_games/cartographer_player_profile/cartographer_player_profile.h"
#include "units/units.h"
#include "units/unit_definitions.h"

#include "saved_games/game_variant.h"

/* constants */

constexpr real32 k_player_default_field_of_view = 78.f;
constexpr real32 k_player_default_field_of_view_radians = DEGREES_TO_RADIANS(k_player_default_field_of_view);

/* globals */

static real32 g_player_control_dt = 0.0f;

/* public code */

void __cdecl player_control_update(real32 world_seconds_elapsed, real32 game_seconds_elapsed)
{
	g_player_control_dt = world_seconds_elapsed;
	INVOKE(0x93857, 0x4BB57, player_control_update, world_seconds_elapsed, game_seconds_elapsed);
	return;
}

real32 __cdecl player_control_get_field_of_view(uint32 user_index)
{
	const s_player_control* player_control = player_control_get(user_index);

	real32 result = observer_default_field_of_view();

	if (player_control->unit_datum_index != NONE)
	{
		real32 fov;
		const s_saved_game_cartographer_player_profile* profile_settings = cartographer_player_profile_get_by_user_index(user_index);
		const s_game_variant* variant = get_game_variant();

		if (game_is_multiplayer() && variant && variant->cartographer_settings.flags.test(_cartographer_variant_force_default_fov))
		{
			fov = k_player_default_field_of_view_radians;
		}
		else if (profile_settings)
		{
			fov = DEGREES_TO_RADIANS(profile_settings->field_of_view);
		}
		else
		{
			unit_datum const* unit = unit_get(player_control->unit_datum_index);
			struct unit_definition const* unit_definition = unit_definition_get(unit->definition_index);
			
			fov = unit_definition->unit.camera_field_of_view;
		}

		result = unit_get_field_of_view(player_control->unit_datum_index, fov, player_control->control_state.desired_zoom_level);
	}
	return result;
}

void __cdecl player_control_get_facing_direction(uint32 player_index, real_vector3d* out_facing)
{
	INVOKE(0x9076E, 0, player_control_get_facing_direction, player_index, out_facing);
}

real_euler_angles2d* __cdecl player_control_get_facing(uint32 user_index)
{
	return INVOKE(0x90759, 0, player_control_get_facing, user_index);
}

void __cdecl player_control_get_camera_info(uint32 user_index, s_player_control_camera_info* camera_info)
{
	INVOKE(0x90BAF, 0, player_control_get_camera_info, user_index, camera_info);
}

real32 __cdecl player_control_get_autocenter_delta()
{
	if (halo_interpolator_update_in_progress())
	{
		return game_tick_length();
	}

	return g_player_control_dt;
}

s_player_control_globals* player_control_globals_get(void)
{
	return *Memory::GetAddress<s_player_control_globals**>(0x4CA37C, 0x4CF0E8);
}

s_player_control* player_control_get(int32 user_index)
{
	return &player_control_globals_get()->local_players[user_index];
}

int16 player_control_get_zoom_level(int32 user_index)
{
	return player_control_get(user_index)->control_state.desired_zoom_level;
}

void player_control_disable_local_camera(bool state)
{
	player_control_globals_get()->disable_camera = state;
}

real32 local_player_held_zoom_delta_time[4]
{
	0.f, 0.f, 0.f, 0.f
};

void __cdecl update_player_control_zoom_updates_held(player_datum* player, s_player_control* player_control, real32 delta)
{
	if (player->user_index != NONE)
	{
		if (local_player_held_zoom_delta_time[player->user_index] <= 1.0f && delta > 0.f)
		{
			local_player_held_zoom_delta_time[player->user_index] += delta;
		}
	}
}


void __cdecl update_player_control_check_held_time(player_datum* player, s_player_control* player_control)
{
	if (player->user_index != NONE)
	{
		if (player_control->zoom_input_held && local_player_held_zoom_delta_time[player->user_index] >= 1.0f)
		{
			player_control->control_state.desired_zoom_level = player_control->field_9A;
		}
	}
}

int16 __cdecl unit_rotate_zoom_level_hook(datum unit_index, int16 zoom_level)
{
	int16 result = INVOKE(0x1413CE, 0, unit_rotate_zoom_level_hook, unit_index, zoom_level);

	if (result != NONE)
	{
		unit_datum* unit = unit_get(unit_index);
		if (unit->unit.player_index != NONE)
		{
			player_datum* player = player_get(unit->unit.player_index);
			int16 user_index = player->user_index;
			if (user_index != NONE)
			{
				local_player_held_zoom_delta_time[user_index] = 0.f;
			}
		}
	}

	return result;
}

__declspec(naked) void update_player_control_zoom_updates_held_jmp()
{
#define pushadoffset 8 * 4
#define pushfdoffset 4
	__asm
	{
		pushad
		pushfd

		mov ebx, [esp + pushfdoffset + pushadoffset + 78h] // move current delta into ebx

		push ebx
		push ebp // push pointer to s_player_control
		push esi // push pointer to player_datum

		call update_player_control_zoom_updates_held

		add esp, 4 * 3

		popfd
		popad

		ret
	}
#undef pushadoffset
#undef pushfdoffset
}

__declspec(naked) void update_player_control_check_held_time_jmp()
{
#define pushadoffset 8 * 4
#define pushfdoffset 4
	__asm
	{
		pushad
		pushfd

		push ebp // push pointer to s_player_control
		push esi // push pointer to player_datum

		call update_player_control_check_held_time

		add esp, 4 * 2

		popfd
		popad

		ret
	}
#undef pushadoffset
#undef pushfdoffset
}

void player_control_apply_patches()
{
	// TODO remove codecaves arnd rewrite the enitre player_control_update_player function

	Codecave(Memory::GetAddress(0x9351A), update_player_control_zoom_updates_held_jmp, 2);
	Codecave(Memory::GetAddress(0x93542), update_player_control_check_held_time_jmp, 2);

	WriteValue<uint8>(Memory::GetAddress(0x93525), 0xEB); // Force the codecaved cmp to always jump
	WriteValue<uint8>(Memory::GetAddress(0x93549), 0xEB); // Force the codecaved cmp to always jump

	PatchCall(Memory::GetAddress(0x9349C), unit_rotate_zoom_level_hook);

	PatchCall(Memory::GetAddress(0x91149), player_control_get_autocenter_delta);
}
