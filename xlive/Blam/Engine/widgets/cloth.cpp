#include "stdafx.h"
#include "cloth.h"

#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/memory/data.h"
#include "Blam/Engine/hs/hs.h"
#include "Util/Hooks/Hook.h"

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
	s_data_iterator<s_cloth> cloth_it(*Memory::GetAddress<s_data_array**>(0x4E66A8));

	while (cloth_it.get_next_datum())
	{
		s_cloth* cloth = cloth_it.get_current_datum();
		if (time_globals::available())
		{
			cloth->accum += dt;
			if (cloth->accum > time_globals::get_seconds_per_tick())
			{
				cloth->accum -= (int32)(cloth->accum / time_globals::get_seconds_per_tick()) * time_globals::get_seconds_per_tick();
				sub_58F279(cloth_it.get_current_datum_index());
				sub_58E82F(cloth_it.get_current_datum_index());
				sub_58EF0E(cloth_it.get_current_datum_index());
			}
			sub_58EE82(cloth_it.get_current_datum_index());
		}
	}
}

void cloth_apply_patches(void)
{
	// disable cloth debugging that writes to cloth.txt
	WriteValue<bool>(Memory::GetAddress(0x41F650), false);

	DETOUR_ATTACH(p_cloth_frame_advance, Memory::GetAddress<t_cloth_frame_advance>(0x18F6D0), cloth_frame_advance);

	return;
}
