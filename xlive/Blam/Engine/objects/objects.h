#pragma once
#include "Blam/Math/BlamMath.h"
#include "Blam/Engine/game/aim_assist.h"
#include "Blam/Engine/memory/data.h"
#include "Blam/Engine/Simulation/SimulationWorld.h"
#include "Blam/Engine/objects/object_placement.h"

#define k_maximum_objects_per_map 2048

enum e_game_team : short
{
	// MP
	_game_team_red = 0,
	_game_team_blue = 1,
	_game_team_yellow = 2,
	_game_team_green = 3,
	_game_team_purple = 4,
	_game_team_orange = 5,
	_game_team_brown = 6,
	_game_team_pink = 7,
	_game_team_neutral = 8,

	// SP
	_game_team_default = 0,
	_game_team_player = 1,
	_game_team_human = 2,
	_game_team_covenant = 3,
	_game_team_flood = 4,
	_game_team_sentinel = 5,
	_game_team_heretic = 6,
	_game_team_prophet = 7,

	// unassigned team ids
	_game_team_unused8 = 8,
	_game_team_unused9 = 9,
	_game_team_unused10 = 10,
	_game_team_unused11 = 11,
	_game_team_unused12 = 12,
	_game_team_unused13 = 13,
	_game_team_unused14 = 14,
	_game_team_unused15 = 15,

	// Shared
	_game_team_none = -1
};

enum e_object_type : signed char
{
	object_type_biped = 0,
	object_type_vehicle = 1,
	object_type_weapon = 2,
	object_type_equipment = 3,
	object_type_garbage = 4,
	object_type_projectile = 5,
	object_type_scenery = 6,
	object_type_machine = 7,
	object_type_control = 8,
	object_type_light_fixture = 9,
	object_type_sound_scenery = 10,
	object_type_crate = 11,
	object_type_creature = 12,
	object_type_none = -1
};

enum e_unit_weapons
{
	PrimaryWeapon,
	SecondaryWeapon,
	DualWeildWeapon
};

enum e_grenades : BYTE
{
	Fragmentation,
	Plasma
};

enum e_weapon_index : WORD
{
	Primary = 0xFF00,
	Secondary = 0xFF01,
	DualWeild = 0x0201
};

enum e_biped_physics_mode : BYTE
{
	mode_ground = 1,
	mode_flying,
	mode_dead,
	mode_posture,
	mode_climbing,
	mode_melee
};

#pragma pack(push, 1)
struct s_object_data_definition
{
	datum tag_definition_index;
	DWORD object_flags;
	char gap_8[4];
	datum next_index;
	datum current_weapon_datum;		//maybe attachment or child
	datum parent_datum;
	WORD unit_in_vehicle_flag;
	INT16 placement_index;
	char gap_1C[12];
	DWORD location[2];
	real_point3d center;
	float radius;
	DWORD gap_3[9];
	real_point3d position;
	real_vector3d orientation;
	real_vector3d up;
	real_point3d translational_velocity;
	real_vector3d angular_velocity;
	float scale;
	datum unique_id;
	__int16 origin_bsp_index;
	e_object_type object_type;//TODO: confirm if its object_type or object_type_flags
	char gap_AB[1];
	__int16 name_list_index;
	char gap_5[1];
	char netgame_equipment_index;
	char placement_policy;
	char gap_6[3];
	datum havok_datum;
	char gap_B8[8];
	WORD field_C0;
	WORD field_C2;
	DWORD field_C4;
	datum damage_owner_object_datum;
	unsigned int field_CC;
	__int16 field_D0;
	BYTE model_variant_id;//hlmt variant tag_block index
	char gap_D3;
	unsigned int simulation_entity_index;
	char field_D8;
	char gap_D9[7];
	WORD destroyed_constraints_flag;
	WORD loosened_constraints_flag;
	float body_max_vitality;
	float shield_max_vitality;
	float body_current_vitality;
	float shield_current_vitality;
	DWORD gap_F4[4];
	WORD shield_stun_ticks;
	WORD body_stun_ticks;
	char gap_108[2];
	WORD field_10A;		//(field_10A & 4) != 0 -- > object_is_dead
	PAD(8);
	__int16 node_buffer_size;
	__int16 nodes_offset;
	PAD(20);
	// PAD(32);
};
#pragma pack(pop)
CHECK_STRUCT_OFFSET(s_object_data_definition, node_buffer_size, 0x114);
CHECK_STRUCT_OFFSET(s_object_data_definition, nodes_offset, 0x116);
CHECK_STRUCT_SIZE(s_object_data_definition, 0x12C);

struct s_unit_data_definition : s_object_data_definition
{
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

struct s_biped_data_definition : s_unit_data_definition
{
	PAD(0x3F4 - sizeof(s_unit_data_definition));
	e_biped_physics_mode biped_mode;//0x3F4
	PAD(0x480 - 0x3F5);

	// NEW DATA
	string_id variant_name;
};
CHECK_STRUCT_SIZE(s_biped_data_definition, 0x480 + 4);

struct s_weapon_data_definition : s_object_data_definition
{
	PAD(0x25C - sizeof(s_object_data_definition));
};
CHECK_STRUCT_SIZE(s_weapon_data_definition, 0x25C);



enum e_object_header_flag :BYTE
{
	_object_header_active_bit = 0x1,
	_object_header_requires_motion_bit = 0x2,
	object_header_flags_4 = 0x4,
	_object_header_being_deleted_bit = 0x8,
	object_header_flags_10 = 0x10,
	_object_header_connected_to_map_bit = 0x20,
	_object_header_child_bit = 0x40,
};


struct s_object_header {
	__int16 datum_salt; //0x00
	e_object_header_flag flags; // 0x02
	e_object_type type; // 0x03
	__int16 unk__;  // 0x04
	__int16 object_data_size;  //0x06
	char* object; //0x08 -
};
CHECK_STRUCT_SIZE(s_object_header, 0xC);

static s_data_array* get_objects_header()
{
	return *Memory::GetAddress<s_data_array**>(0x4E461C, 0x50C8EC);
};

// Gets the header of the object, containing some details
static s_object_header* get_objects_header(datum object_idx)
{
	auto objects_header = get_objects_header();
	return (s_object_header*)(&objects_header->data[objects_header->datum_element_size * DATUM_INDEX_TO_ABSOLUTE_INDEX(object_idx)]);
}

// Get the object fast, with no validation from datum index
template<typename T = s_object_data_definition>
static T* object_get_fast_unsafe(datum object_idx)
{
	return (T*)get_objects_header(object_idx)->object;
}

static real_matrix4x3* get_object_nodes(datum object_idx, int* out_node_count)
{
	auto object = object_get_fast_unsafe(object_idx);
	*out_node_count = object->node_buffer_size / sizeof(real_matrix4x3);
	return (real_matrix4x3*)((char*)object + object->nodes_offset);
}

// Gets the object and verifies the type, returns NULL if object doesn't match object type flags
template<typename T = s_object_data_definition>
static T* object_try_and_get_and_verify_type(datum object_idx, int object_type_flags)
{
	auto p_object_try_and_get_and_verify_type = Memory::GetAddress<char* (__cdecl*)(datum, int)>(0x1304E3, 0x11F3A6);
	return (T*)p_object_try_and_get_and_verify_type(object_idx, object_type_flags);
}

// TODO remove
namespace Engine::Objects
{
	void create_new_placement_data(s_object_placement_data* object_placement_data, datum object_definition_idx, datum object_owner_idx, int a4);
	datum object_new(s_object_placement_data* object_placement_data);
	void apply_biped_object_definition_patches();
	void simulation_action_object_create(datum object_idx);
	void object_destroy(datum object_idx);
	int object_get_count();
	int object_count_from_iter();
}

datum object_get_damage_owner(datum damaged_unit_index);
