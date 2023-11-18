#include "stdafx.h"
#include "first_person_weapons.h"

#include "Blam/Engine/game/cheats.h"
#include "Util/Hooks/Hook.h"

bool show_first_person = true;

s_first_person_weapon* first_person_weapon_get_global(void)
{
	return *Memory::GetAddress<s_first_person_weapon**>(0x977104, 0x99FBC4);
}

s_first_person_weapon* first_person_weapons_get(uint32 user_index)
{
	return &first_person_weapon_get_global()[user_index];
}

s_first_person_orientations* first_person_orientations_get_global(void)
{
	return *Memory::GetAddress<s_first_person_orientations**>(0x977100, 0x99FBC0);
}

s_first_person_orientations* first_person_orientations_get(uint32 user_index, uint32 weapon_slot)
{
	return &first_person_orientations_get_global()[(k_first_person_max_weapons * user_index) + weapon_slot];
}

s_first_person_model_data* first_person_model_data_get_global(void)
{
	return Memory::GetAddress<s_first_person_model_data*>(0x4E8F48, 0x50EDF0);
}

s_first_person_model_data* first_person_model_data_get(uint32 user_index)
{
	return &first_person_model_data_get_global()[user_index];
}

bool __cdecl render_first_person_check(e_skull_type skull_type)
{
	return ice_cream_flavor_available(skull_type) || !show_first_person;
}

void toggle_first_person(bool state)
{
	show_first_person = state;
	return;
}

void first_person_weapons_apply_patches()
{
	// Only patch if not a dedi
	if (!Memory::IsDedicatedServer()) 
	{ 
		PatchCall(Memory::GetAddress(0x228579), render_first_person_check);
	}

	return;
}
