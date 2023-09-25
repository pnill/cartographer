#pragma once
#include "damage.h"
#include "object_types.h"

#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Math/BlamMath.h"

enum e_object_source : char
{
	object_source_structure = 0,
	object_source_editor = 1,
	object_source_dynamic = 2,
	object_source_legacy = 3,
	object_source_none = -1
};

enum e_bsp_policy : char
{
	bsp_policy_default = 0,
	bsp_policy_always_placed = 1,
	bsp_policy_manual_bsp_placement = 2
};

struct s_location
{
	DWORD leaf_index;
	short cluster;
	WORD bsp_index;
};

struct s_placement_info
{
	WORD origin_bsp_index;
	e_object_type object_type;
	byte placement_type;
};

struct object_placement_data
{
	datum tag_index;
	DWORD unique_id;
	s_placement_info placement_info;
	string_id variant_name;
	DWORD placement_index;
	BYTE placement_policy;
	BYTE unk_15;
	WORD unk_16;
	DWORD object_placement_flags;
	real_point3d position;
	real_vector3d forward;
	real_vector3d up;
	real_vector3d translational_velocity;
	real_vector3d angular_velocity;
	float scale;
	int player_index;
	datum unit_datum;
	int team_index;
	s_damage_owner damage_owner;
	WORD field_72;
	DWORD active_change_colors_mask;
	real_color_rgb change_colors[4];
	DWORD foreground_emblem;
	DWORD region_index;
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