#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Engine/math/real_math.h"

enum e_placement_flags : __int32
{
	_placement_flag_not_automatically_bit = FLAG(0),
	_placement_flag_not_on_easy_bit = FLAG(1),
	_placement_flag_not_on_normal_bit = FLAG(2),
	_placement_flag_not_on_hard_bit = FLAG(3),
	_placement_flag_lock_type_to_env_object_bit = FLAG(4),
	_placement_flag_lock_transform_to_env_object_bit = FLAG(5),
	_placement_flag_never_placed_bit = FLAG(6),
	_placement_flag_lock_name_to_env_object_bit = FLAG(7),
	_placement_flag_create_at_rest_bit = FLAG(8),
};

enum e_transform_flags : __int16
{
	_transform_flag_mirrored_bit = FLAG(0),
};

enum e_source : __int8
{
	source_structure = 0,
	source_editor = 1,
	source_dynamic = 2,
	source_legacy = 3,
};

enum e_bsp_policy : __int8
{
	_bsp_policy_default_bit = FLAG(0),
	_bsp_policy_always_placed_bit = FLAG(1),
	_bsp_policy_manual_bsp_placement_bit = FLAG(2),
};

enum e_active_change_colors : __int32
{
	_active_change_colors_primary_bit = FLAG(0),
	_active_change_colors_secondary_bit = FLAG(1),
	_active_change_colors_tertiary_bit = FLAG(2),
	_active_change_colors_quaternary_bit = FLAG(3),
};

struct s_location
{
	DWORD leaf_index;
	short cluster;
	WORD bsp_index;
};

struct s_scenario_group_placement_object_block
{
	__int16 type;
	__int16 name;
	e_placement_flags placement_flags;
	real_point3d position;
	real_vector3d rotation;
	float scale;
	e_transform_flags transform_flags;
	__int16 manual_bsp_flags;
	int unique_id;
	__int16 origin_bsp_index;
	byte object_type;
	e_source source;
	e_bsp_policy bsp_policy;
	BYTE pad;
	__int16 editor_folder;
};

struct s_scenario_object : s_scenario_group_placement_object_block
{
	string_id variant_name;
	e_active_change_colors active_change_colors;
	D3DCOLOR primary_color;
	D3DCOLOR secondary_color;
	D3DCOLOR tertiary_color;
	D3DCOLOR quaternary_color;
	WORD pathfinding_policy;
	WORD lightmapping_policy;
	tag_block<> pathfinding_references;		// TODO setup properly
	WORD unk;
	WORD valid_multiplayer_games;
};

void location_invalidate(s_location* object_location);
void scenario_location_from_point(s_location* bsp_point, const real_point3d* point);
void scenario_location_from_leaf(s_location* location, DWORD leaf_index);
