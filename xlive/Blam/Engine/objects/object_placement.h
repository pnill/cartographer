#pragma once
#include "damage.h"
#include "emblems.h"
#include "object_identifier.h"

#include "math/color_math.h"
#include "memory/static_arrays.h"
#include "tag_files/string_id.h"

enum e_bsp_policy : int8
{
	_bsp_policy_default = 0,
	_bsp_policy_always_placed = 1,
	_bsp_policy_manual_bsp_placement = 2
};

struct s_location
{
	int32 leaf_index;
	int16 cluster_index;
	int16 bsp_index;
};

enum e_scenario_object_placement_flags : uint32
{
	_scenario_object_placement_bit_0 = 0,
	_scenario_object_placement_bit_1 = 1,
	_scenario_object_placement_bit_2 = 2,
	_scenario_object_placement_bit_3 = 3,
	_scenario_object_placement_bit_4 = 4,
	_scenario_object_placement_bit_5 = 5,
	_scenario_object_placement_bit_6 = 6,
	_scenario_object_placement_bit_7 = 7,
	_scenario_object_placement_bit_8 = 8,
	_scenario_object_placement_bit_9 = 9,
	_scenario_object_placement_bit_10 = 10,
	k_scenario_object_placement_flags
};

struct object_placement_data
{
	datum tag_index;
	c_object_identifier object_identifier;
	string_id variant_name;
	uint32 scenario_datum_index;
	e_bsp_policy placement_policy;
	BYTE unk_15;
	WORD unk_16;
	c_flags_no_init<e_scenario_object_placement_flags, uint32, k_scenario_object_placement_flags> flags;
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
	real_rgb_color change_colors[4];
	s_emblem_info emblem_info;
	int8 pad1;
	int32 region_index;
	WORD destroyed_constraints_flag;
	WORD loosened_constraints_flag;
	WORD field_B4;
	WORD field_B6;
	bool object_is_inside_cluster;
	BYTE unk_22;
	WORD unk_23;
	s_location location;
};
ASSERT_STRUCT_SIZE(object_placement_data, 0xC4);