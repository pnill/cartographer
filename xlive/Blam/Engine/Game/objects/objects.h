#pragma once

#include "Blam/Math/BlamMath.h"
#include "Blam/Engine/DataArray/DataArray.h"
#include "Blam/Engine/Game/objects/object_placement.h"
#include "Blam/Engine/Players/PlayerActions.h"

enum e_object_team : BYTE
{
	// MP
	Red = 0,
	Blue = 1,
	Yellow = 2,
	Green = 3,
	Purple = 4,
	Orange = 5,
	Bronwn = 6,
	Pink = 7,
	// SP
	Default = 0,
	player = 1,
	Human = 2,
	Covenant = 3,
	Flood = 4,
	Sentinel = 5,
	Heretic = 6,
	Prophet = 7,

	// unasigned team ids
	Unused8 = 8,
	Unused9 = 9,
	Unused10 = 10,
	Unused11 = 11,
	Unused12 = 12,
	Unused13 = 13,
	Unused14 = 14,
	Unused15 = 15,

	// Shared
	None = 255
};

enum e_object_type : signed char
{
	biped = 0,
	vehicle,
	weapon,
	equipment,
	garbage,
	projectile,
	scenery,
	machine,
	control,
	light_fixture,
	sound_scenery,
	crate,
	creature,
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
	DWORD field_C8;
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

namespace objects
{
	void create_new_placement_data(s_object_placement_data* object_placement_data, datum object_definition_idx, datum object_owner_idx, int a4);
	datum object_new(s_object_placement_data* object_placement_data);
	void apply_biped_object_definition_patches();
	void simulation_action_object_create(datum object_idx);
	void object_destroy(datum object_idx);
	int object_get_count();
	int object_count_from_iter();
}
