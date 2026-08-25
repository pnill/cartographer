#include "stdafx.h"
#include "cloth.h"

#include "game/game_time.h"
#include "memory/data.h"
#include "objects/objects.h"

data_array* cloth_get_data_array(void)
{
	return *Memory::GetAddress<data_array**>(0x4E66A8);
}

void __cdecl sub_58F279(datum cloth_index)
{
	INVOKE(0x18F279, 0, sub_58F279, cloth_index);
}

void __cdecl sub_58E82F(datum cloth_index)
{
	void* func = Memory::GetAddress<void*>(0x18E82F);
	__asm
	{
		lea eax, cloth_index
		mov eax, dword ptr[eax]
		call func
	}
}

void __cdecl sub_58EF0E(datum cloth_index)
{
	void* func = Memory::GetAddress<void*>(0x18EF0E);
	__asm
	{
		lea eax, cloth_index
		mov eax, dword ptr[eax]
		call func
	}
}

void __cdecl sub_58EE82(datum cloth_index)
{
	INVOKE(0x18EE82, 0, sub_58EE82, cloth_index);
}

typedef void(__cdecl* t_cloth_frame_advance)(real32 dt);
t_cloth_frame_advance p_cloth_frame_advance;

void cloth_frame_advance(real32 dt)
{
	data_array* cloth_array = cloth_get_data_array();

	for (datum index = data_next_index(cloth_array, NONE); index != NONE; index = data_next_index(cloth_array, index))
	{
		if (game_time_initialized())
		{
			s_cloth* cloth = (s_cloth*)datum_get(cloth_array, index);
			cloth->accum += dt;

			if (cloth->accum > game_tick_length())
			{
				cloth->accum -= (int32)(cloth->accum / game_tick_length()) * game_tick_length();
				sub_58F279(index);
				sub_58E82F(index);
				sub_58EF0E(index);
			}
			sub_58EE82(index);
		}
	}
}

void cloth_apply_patches(void)
{
	// disable cloth debugging that writes to cloth.txt
	*Memory::GetAddress<bool*>(0x41F650) = false;

	DETOUR_ATTACH(p_cloth_frame_advance, Memory::GetAddress<t_cloth_frame_advance>(0x18F6D0), cloth_frame_advance);

	// replace functions with interpolated equivalents
	PatchCall(Memory::GetAddress(0x18E19C, 0x0), object_get_markers_by_string_id);
	PatchCall(Memory::GetAddress(0x18E4DD, 0x0), object_get_markers_by_string_id);

	return;
}
