#pragma once
#include "damage.h"
#include "emblems.h"
#include "object_identifier.h"

#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Math/BlamMath.h"

enum e_bsp_policy : int8
{
	_bsp_policy_default = 0,
	_bsp_policy_always_placed = 1,
	_bsp_policy_manual_bsp_placement = 2
};

struct s_location
{
	DWORD leaf_index;
	short cluster;
	WORD bsp_index;
};

struct object_placement_data
{
	datum tag_index;
	c_object_identifier object_identifier;
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
	DWORD active_change_colors_mask;
	real_color_rgb change_colors[4];
	s_emblem_info emblem_info;
	int8 pad;
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