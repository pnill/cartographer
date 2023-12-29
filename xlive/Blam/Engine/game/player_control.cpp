#include "stdafx.h"
#include "player_control.h"

#include "players.h"
#include "Util/Hooks/Hook.h"
#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/main/interpolator.h"

real32 g_player_control_dt = 0.0f;

void player_control_update_dt(real32 dt)
{
	g_player_control_dt = dt;
}

real32 __cdecl player_control_get_autocenter_delta()
{
	if (halo_interpolator_update_in_progress())
	{
		return time_globals::get_seconds_per_tick();
	}

	return g_player_control_dt;
}

s_player_control_globals* s_player_control_globals::get()
{
	return *Memory::GetAddress<s_player_control_globals**>(0x4CA37C, 0x4CF0E8);
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
			player_control->actions.weapon_indexes.primary_weapon_index = player_control->gap_9A[0];
			player_control->actions.weapon_indexes.secondary_weapon_index = player_control->gap_9A[1];
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
			int16 local_user_index = player->user_index;
			if(local_user_index != NONE)
				local_player_held_zoom_delta_time[local_user_index] = 0.f;
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

		mov ebx, [esp + pushfdoffset + pushadoffset + 78h] // move current delta onto ebx

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
