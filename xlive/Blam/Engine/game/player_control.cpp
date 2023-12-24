#include "stdafx.h"
#include "player_control.h"

#include "players.h"
#include "Blam/Engine/units/bipeds.h"
#include "Util/Hooks/Hook.h"
#include <H2MOD.h>

s_player_control_globals* s_player_control_globals::get()
{
	return *Memory::GetAddress<s_player_control_globals**>(0x4CA37C, 0x4CF0E8);
}

real32 local_player_held_zoom_delta_time[4]
{
	0.f, 0.f, 0.f, 0.f
};

void __cdecl update_player_control_zoom_updates_held(int32 player_index, s_player_control* player_control, real32 delta)
{
	if (local_player_held_zoom_delta_time[player_index] <= 1.0f && delta > 0.f)
	{
		local_player_held_zoom_delta_time[player_index] += delta;
	}
}


void __cdecl update_player_control_check_held_time(int32 player_index, s_player_control* player_control)
{
	if(player_control->zoom_input_held && local_player_held_zoom_delta_time[player_index] >= 1.0f)
	{
		player_control->actions.weapon_indexes.primary_weapon_index = player_control->gap_9A[0];
		player_control->actions.weapon_indexes.secondary_weapon_index = player_control->gap_9A[1];
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
	DWORD vebx;
	__asm
	{
		// Store original registers
		mov vebx, ebx
		push eax
		push ebx
		push ecx

		mov ebx, ebp			// move pointer to s_player_control onto ebx
		mov eax, [esp + 84h]	// move current delta onto eax
		mov ecx, [esp + 70h]	// move player_index onto ecx

		// push arguments to stack
		push eax
		push ebx
		push ecx

		call update_player_control_zoom_updates_held

		// adjust stack
		add esp, 4 * 3

		// restore original registers
		pop eax
		pop ebx
		pop ecx
		mov ebx, vebx
		ret
	}
}

__declspec(naked) void update_player_control_check_held_time_jmp()
{
	DWORD vebx;
	__asm
	{
		// store original register
		mov vebx, ebx
		push ebx
		push eax

		mov ebx, ebp			// move pointer to s_player_control onto ebx
		mov eax, [esp + 70H]	// move player_index onto eax

		// push arguments to stack
		push ebx
		push eax

		call update_player_control_check_held_time

		// adjust stack
		add esp, 4 * 2

		// restore original register
		pop ebx
		pop eax
		mov ebx, vebx
		ret
	}
}

void apply_player_control_patches()
{
	// TODO remove codecaves arnd rewrite the enitre player_control_update_player function

	Codecave(Memory::GetAddress(0x9351A), update_player_control_zoom_updates_held_jmp, 2);
	Codecave(Memory::GetAddress(0x93542), update_player_control_check_held_time_jmp, 2);

	WriteValue<uint8>(Memory::GetAddress(0x93525), 0xEB); // Force the codecaved cmp to always jump
	WriteValue<uint8>(Memory::GetAddress(0x93549), 0xEB); // Force the codecaved cmp to always jump

	PatchCall(Memory::GetAddress(0x9349C), unit_rotate_zoom_level_hook);
}
