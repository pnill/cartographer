#pragma once

#include "Blam\Math\BlamMath.h"

#include "Blam\Engine\DataArray\DataArray.h"

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
struct s_object_base_definition
{
	datum tag_definition_index;
	DWORD object_flags;
	BYTE gap_8[4];
	datum next_index;
	datum current_weapon_datum;
	datum parent_index;
	WORD unit_in_vehicle_flag;
	INT16 placement_index;
	BYTE gap_1C[12];
	DWORD location[2];
	real_point3d center;
	float radius;
	DWORD gap_3[9];
	real_point3d placement;
	real_vector3d orientation;
	real_vector3d up;
	real_point3d translational_velocity;
	real_vector3d angular_velocity;
	float scale;
	BYTE gap_A4[6];
	e_object_type object_type;
	BYTE gap_AB[1];
	__int16 name_list_index;
	BYTE gap_5[1];
	BYTE netgame_equipment_index;
	DWORD gap_6;
	datum havok_component_datum;
	BYTE gap_B8[0x1A];
	BYTE model_variant_id;
	BYTE gap_D6[17];
	float body_max_vitality;
	float shield_max_vitality;
	float body_current_vitality;
	float shield_current_vitality;
	DWORD gap_F4[4];
	float shield_recharge_timer;
	BYTE gap_108[2];
	WORD field_10A;
	BYTE gap_10C[32];
};
#pragma pack(pop)
CHECK_STRUCT_SIZE(s_object_base_definition, 0x12C);

struct s_biped_object_definition : s_object_base_definition
{
	BYTE ObjectsAttach;//0x12C
	BYTE unk_11[3];//0x12D
	datum ActorDatum; // 0x130
	BYTE unk_17[4]; //0x138
	DWORD Flags;//0x138
	e_object_team Team;//0x13C
	WORD unk_12;//0x13D
	datum PlayerDatum;//0x140
	BYTE unk_13[9];//0x144
	WORD CrouchJumpRelated;//0x150
	float UnitShoot;//0x152
	BYTE unk_14[102];//0x156
	float forward_movement_speed;//0x1BC
	float left_movement_speed;//0x1C0
	BYTE unk_15[16];//0x1C4
	datum TargetObject;//0x1D4
	BYTE unk_16[82];//0x1D8
	e_weapon_index UnitSwitchWeapon;//0x22A
	datum PrimaryWeapon; // 0x22C
	datum SecondaryWeapon; // 0x230
	datum DualWieldWeapon; // 0x234
	BYTE pad[0x18]; //  0x238 

	BYTE CurrentGrenadesIndex; //0x250
	BYTE CurrentGrenadesIndex2; //0x251
	BYTE Frag_Grenades; //0x252
	BYTE Plasma_Grenades; //0x253

	float ActiveCamoFlagePower;//0x2C4
	float ActiveCamoFlageTimer;//0x2C8
	float ActiveCamoFlageDepletionPower;//0x2CC

	e_biped_physics_mode unitState;//0x3F4
	BYTE unk_18[0x21C];
	
	// NEW DATA
	int variant_index;
};
CHECK_STRUCT_SIZE(s_biped_object_definition, 0x480 + 4);

struct s_weapon_object_definition : s_object_base_definition
{
	char gap[0x25C - sizeof(s_object_base_definition)];
};
CHECK_STRUCT_SIZE(s_weapon_object_definition, 0x25C);

struct s_object_header {
	__int16 datum_salt; //0x00
	BYTE flags; // 0x02
	e_object_type type; // 0x03
	__int16 unk__;  // 0x04
	__int16 unk_size;  //0x06
	char* object; //0x08 - 
};
CHECK_STRUCT_SIZE(s_object_header, 0xC);

static s_data_array* get_objects_header()
{
	return *Memory::GetAddress<s_data_array**>(0x4E461C, 0x50C8EC);
};

// Gets the header of the object, containing some details
static s_object_header* get_objects_header(datum object_index)
{
	auto objects_header = get_objects_header();
	return (s_object_header*)(&objects_header->data[objects_header->datum_element_size * DATUM_ABSOLUTE_INDEX(object_index)]);
}

// Get the object fast, with no validation from datum index
template<typename T = s_object_base_definition>
static T* object_get_fast_unsafe(datum object_index)
{
	return (T*)get_objects_header(object_index)->object;
}

// Gets the object and verifies the type, returns NULL if object doesn't match object type flags
template<typename T = s_object_base_definition>
static T* object_try_and_get_and_verify_type(datum object_index, int object_type_flags)
{
	auto p_object_try_and_get_and_verify_type = Memory::GetAddress<char*(__cdecl*)(datum, int)>(0x1304E3, 0x11F3A6);
	return (T*)p_object_try_and_get_and_verify_type(object_index, object_type_flags);
}
