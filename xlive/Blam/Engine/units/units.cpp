#include "stdafx.h"
#include "units.h"

void __cdecl unit_delete_all_weapons(datum unit_datum_index)
{
	INVOKE(0x1441E0, 0x133030, unit_delete_all_weapons, unit_datum_index);
	return;
}

datum __cdecl unit_inventory_next_weapon(datum unit_datum_index)
{
	return INVOKE(0x139E04, 0x128C53, unit_inventory_next_weapon, unit_datum_index);
}

datum __cdecl unit_inventory_get_weapon(uint16 absolute_unit_index, int16 weapon_slot)
{
	//todo get server offset
	return INVOKE(0x13D9AD, 0, unit_inventory_get_weapon, absolute_unit_index, weapon_slot);
}

bool __cdecl unit_add_weapon_to_inventory(datum unit_datum_index, datum weapon_datum_index, e_weapon_addition_method weapon_addition_method)
{
	return INVOKE(0x1442AA, 0x1330FA, unit_add_weapon_to_inventory, unit_datum_index, weapon_datum_index, weapon_addition_method);
}

float __cdecl unit_get_field_of_view(datum unit_datum_index, real32 unit_camera_field_of_view, s_player_action_weapons weapon_index)
{
	return INVOKE(0x1414AC, 0x1302FB, unit_get_field_of_view, unit_datum_index, unit_camera_field_of_view, weapon_index);
}

bool unit_is_dual_wielding(datum unit_index)
{
	unit_datum* unit = (unit_datum*)object_get_fast_unsafe(unit_index);
	return unit->weapon_indices[0] != NONE && unit->weapon_indices[1] != NONE;
}