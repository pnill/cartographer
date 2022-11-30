#pragma once
#include "Blam/Engine/items/equipment.h"
#include "Blam/Engine/objects/objects.h"
#include "Blam/Engine/math/real_math.h"

struct s_unit_data_definition : s_object_data_definition
{
	char gap_12C[4];
	DWORD field_130;
	datum simulation_actor_index;
	DWORD unit_flags;		  //(unit_data->unit_flags & 8) != 0   -->active_camo_active
	//unit_data->unit_flags |= 2         -->unit_is_alive
	e_object_team unit_team;
	char pad[3];
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

namespace units
{
	void __cdecl remove_equipment(datum unit_idx);
	signed int __cdecl inventory_next_weapon(datum unit_idx);
	bool __cdecl assign_equipment_to_unit(datum unit_idx, datum object_idx, short a3);
	float carto_get_distance(int playerIndex1, int playerIndex2);
	real_point3d* carto_get_player_unit_coords(int playerIndex);
	BYTE* carto_get_player_unit_from_player_index(int playerIndex);
	void carto_call_give_player_weapon(int playerIndex, datum weaponId, bool bReset);
	const wchar_t* carto_get_local_player_name(int local_player_index);
	int carto_get_player_index_from_unit_datum_index(datum unit_datum_index);
	BYTE carto_get_unit_team_index(datum unit_datum_index);
	void carto_set_unit_speed_patch(bool hackit);
	void carto_set_player_unit_grenades_count(int playerIndex, e_grenades type, BYTE count, bool resetEquipment);
	void carto_give_unit_weapon_from_datum(datum unit_datum, datum weapon_tag_index);
}