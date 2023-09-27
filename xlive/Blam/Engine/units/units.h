#pragma once
#include "Blam/Engine/game/player_control.h"
#include "Blam/Engine/objects/objects.h"

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

enum e_unit_weapons
{
	unit_weapons_primary_weapon,
	unit_weapons_secondary_weapon,
	unit_weapons_dual_weild_weapon
};

struct s_unit_data_definition
{
	s_object_data_definition object;
	char gap_12C[4];
	DWORD field_130;
	datum simulation_actor_index;
	DWORD unit_flags;		  //(unit_data->unit_flags & 8) != 0   -->active_camo_active
	//unit_data->unit_flags |= 2         -->unit_is_alive
	e_game_team unit_team;
	char pad[2];
	datum controlling_player_index;
	char gap_142[12];
	DWORD control_flags;
	DWORD control_flags_2;
	DWORD animation_state;
	real_vector3d desired_facing;
	real_vector3d desired_aiming;
	real_vector3d aiming_vector;
	real_vector3d aiming_vector_velocity;
	real_vector3d desired_looking;
	real_vector3d looking_vector;
	real_vector3d looking_vector_velocity;
	DWORD field_1B0;
	DWORD field_1B4;
	DWORD field_1B8;
	real_vector3d throttle;
	char aiming_speed;			//might not be char
	char gap_1C9[3];
	float trigger;
	float secondary_trigger;
	s_aim_assist_targetting_data target_info;
	char gap_1F8[24];
	DWORD parent_seat_index;
	char gap_214[20];
	WORD weapon_set_identifier;
	char gap_22A[39];
	char current_grenade_index;
	WORD grenade_counts_mask;
	char gap_254;
	char zoom_level;
	char gap_256[110];
	float active_camo_power;
	char gap_2C8[4];
	float active_camo_regrowth;
	PAD(144);
};
CHECK_STRUCT_SIZE(s_unit_data_definition, 0x360);

void __cdecl unit_delete_all_weapons(datum unit_datum_index);
datum __cdecl unit_inventory_next_weapon(datum unit_datum_index);
bool __cdecl unit_add_weapon_to_inventory(datum unit_datum_index, datum weapon_datum_index, e_weapon_addition_method weapon_addition_method);
float __cdecl unit_get_field_of_view(datum unit_datum_index, real32 unit_camera_field_of_view, s_player_action_weapons weapon_indexes);
