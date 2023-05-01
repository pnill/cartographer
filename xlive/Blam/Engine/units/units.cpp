#include "stdafx.h"
#include "units.h"

void __cdecl unit_delete_all_weapons(datum unit_datum_index)
{
	typedef void(__cdecl* unit_reset_equipment_t)(datum unit_datum_index);
	auto p_unit_reset_equipment = Memory::GetAddress<unit_reset_equipment_t>(0x1441E0, 0x133030);
	p_unit_reset_equipment(unit_datum_index);
}

datum __cdecl unit_inventory_next_weapon(datum unit_datum_index)
{
	typedef signed int(__cdecl* unit_inventory_next_weapon_t)(datum unit);
	auto p_unit_inventory_next_weapon = Memory::GetAddress<unit_inventory_next_weapon_t>(0x139E04, 0x128C53);
	return p_unit_inventory_next_weapon(unit_datum_index);
}

bool __cdecl unit_add_weapon_to_inventory(datum unit_datum_index, datum weapon_datum_index, e_weapon_addition_method weapon_addition_method)
{
	typedef bool(__cdecl* assign_equipment_to_unit_t)(datum unit_datum_index, datum weapon_datum_index, e_weapon_addition_method weapon_addition_method);
	auto passign_equipment_to_unit = Memory::GetAddress<assign_equipment_to_unit_t>(0x1442AA, 0x1330FA);
	return passign_equipment_to_unit(unit_datum_index, weapon_datum_index, weapon_addition_method);
}

float __cdecl unit_get_field_of_view(const datum unit_datum_index, const float unit_camera_field_of_view, const s_player_action_weapons weapon_index)
{
	typedef float(__cdecl* unit_get_field_of_view_t)(const datum unit_datum_index, const float unit_camera_field_of_view, const s_player_action_weapons weapon_index);
	unit_get_field_of_view_t p_unit_get_field_of_view = Memory::GetAddress<unit_get_field_of_view_t>(0x1414AC, 0x1302FB);
	return p_unit_get_field_of_view(unit_datum_index, unit_camera_field_of_view, weapon_index);
}