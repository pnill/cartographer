#pragma once
#include "Blam/Engine/game/player_control.h"

enum e_weapon_addition_method : int16
{
	_weapon_addition_method_zero = 0,
	_weapon_addition_method_one = 1,
	_weapon_addition_method_two = 2,
	_weapon_addition_method_three = 3,
	_weapon_addition_method_four = 4,
	_weapon_addition_method_five = 5,
	_weapon_addition_method_six = 6,
	_weapon_addition_method_seven = 7,
};

void __cdecl unit_delete_all_weapons(datum unit_datum_index);
datum __cdecl unit_inventory_next_weapon(datum unit_datum_index);
bool __cdecl unit_add_weapon_to_inventory(datum unit_datum_index, datum weapon_datum_index, e_weapon_addition_method weapon_addition_method);
float __cdecl unit_get_field_of_view(datum unit_datum_index, real32 unit_camera_field_of_view, s_player_action_weapons weapon_indexes);
