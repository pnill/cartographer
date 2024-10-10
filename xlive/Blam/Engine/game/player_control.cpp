#include "stdafx.h"
#include "player_control.h"

#include "players.h"
#include "camera/observer.h"

#include "game/game_time.h"
#include "H2MOD/Modules/CustomVariantSettings/CustomVariantSettings.h"
#include "main/interpolator.h"
#include "saved_games/cartographer_player_profile.h"
#include "units/unit_definitions.h"

real32 g_player_control_dt = 0.0f;

void player_control_update_dt(real32 dt)
{
	g_player_control_dt = dt;
}

real32 __cdecl player_control_get_field_of_view(uint32 user_index)
{
	const s_player_control* player_control_info = player_control_get(user_index);

	float result = observer_suggested_field_of_view();

	if (player_control_info->unit_datum_index != NONE)
	{
		float fov;
		const s_saved_game_cartographer_player_profile* profile_settings = cartographer_player_profile_get_by_user_index(user_index);

		if (currentVariantSettings.forced_fov != 0)
		{
			fov = DEGREES_TO_RADIANS(currentVariantSettings.forced_fov);
		}
		else if (profile_settings)
		{
			fov = DEGREES_TO_RADIANS(profile_settings->field_of_view);
		}
		else
		{
			_unit_definition* unit = (_unit_definition*)tag_get_fast(object_get_fast_unsafe(player_control_info->unit_datum_index)->tag_definition_index);
			fov = unit->camera_field_of_view;
		}

		result = unit_get_field_of_view(player_control_info->unit_datum_index, fov, player_control_info->actions.zoom_level);
	}
	return result;
}

void __cdecl player_control_get_facing_direction(uint32 player_index, real_vector3d* out_facing)
{
	INVOKE(0x9076E, 0, player_control_get_facing_direction, player_index, out_facing);
}

real_euler_angles2d* player_control_get_facing(uint32 user_index)
{
	return INVOKE(0x90759, 0, player_control_get_facing, user_index);
}

void player_control_get_camera_info(uint32 user_index, s_player_control_camera_info* camera_info)
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
	return player_control_get(user_index)->actions.zoom_level;
}

void player_control_disable_local_camera(bool state)
{
	player_control_globals_get()->disable_camera = state;
}

real32 local_player_held_zoom_delta_time[4]
{
	0.f, 0.f, 0.f, 0.f
};

void __cdecl update_player_control_zoom_updates_held(s_player* player, s_player_control* player_control, real32 delta)
{
	if (player->user_index != NONE)
	{
		if (local_player_held_zoom_delta_time[player->user_index] <= 1.0f && delta > 0.f)
		{
			local_player_held_zoom_delta_time[player->user_index] += delta;
		}
	}
}


void __cdecl update_player_control_check_held_time(s_player* player, s_player_control* player_control)
{
	if (player->user_index != NONE)
	{
		if (player_control->zoom_input_held && local_player_held_zoom_delta_time[player->user_index] >= 1.0f)
		{
			player_control->actions.zoom_level = player_control->field_9A;
		}
	}
}

int16 __cdecl unit_rotate_zoom_level_hook(datum object_index, __int16 a2)
{
	int16 result = INVOKE(0x1413CE, 0, unit_rotate_zoom_level_hook, object_index, a2);
	if(result != NONE)
	{
		s_player* player = s_player::get_from_unit_index(object_index);
		if (player)
		{
			int16 user_index = player->user_index;
			if(user_index != NONE)
				local_player_held_zoom_delta_time[user_index] = 0.f;
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
		push esi // push pointer to s_player

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
		push esi // push pointer to s_player

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
