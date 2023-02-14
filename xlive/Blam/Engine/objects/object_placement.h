#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Engine/objects/damage.h"
#include "Blam/Math/BlamMath.h"

struct s_location
{
	DWORD leaf_index;
	WORD cluster;
	WORD bsp_index;
};

struct object_placement_data
{
	datum tag_index;//0
	DWORD unique_id;//0x4
	DWORD origin_bsp_index;//0x8
	string_id variant_name;//0xC
	DWORD placement_index;
	BYTE placement_policy;//0x14
	BYTE unk_15;//0x15
	WORD unk_16;//0x16
	DWORD object_placement_flags;//0x18
	real_point3d position;//0x1C
	real_vector3d forward;//0x28
	real_vector3d up;//0x34
	real_point3d translational_velocity;//0x40
	real_vector3d angular_velocity;//0x4C
	float scale;//0x58
	int player_index;
	datum unit_datum;
	int team_index;
	s_damage_owner damage_owner;
	WORD field_72;
	DWORD active_change_colors_mask;
	real_color_rgb change_colors[4];
	DWORD foreground_emblem;
	DWORD unk_AC;
	WORD destroyed_constraints_flag;
	WORD loosened_constraints_flag;
	WORD field_B4;
	WORD field_B6;
	bool object_is_inside_cluster;
	BYTE unk_22;
	WORD unk_23;
	s_location location;
};
CHECK_STRUCT_SIZE(object_placement_data, 0xC4);
