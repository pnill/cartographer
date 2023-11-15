#include "stdafx.h"
#include "first_person_weapons.h"

#include "Blam/Engine/game/cheats.h"
#include "Util/Hooks/Hook.h"

bool show_first_person = true;

typedef bool(__cdecl* render_first_person_check_t)(e_skull_type skull_type);

s_first_person_model_data* first_person_model_data_get(void)
{
	return Memory::GetAddress<s_first_person_model_data*>(0x4E8F48, 0x50EDF0);
}

bool __cdecl render_first_person_check(e_skull_type skull_type)
{
	return ice_cream_flavor_available(skull_type) || !show_first_person;
}

void toggle_first_person(bool state)
{
	show_first_person = state;
}

void first_person_weapons_apply_patches()
{
	if (Memory::IsDedicatedServer()) { return; }

	PatchCall(Memory::GetAddress<render_first_person_check_t>(0x228579), render_first_person_check);
}
