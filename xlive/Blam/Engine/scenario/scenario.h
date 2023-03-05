#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Engine/structures/structures.h"
#include "Blam/Engine/math/real_math.h"

enum class e_placement_flags : __int32
{
	not_automatically = FLAG(0),
	unused = FLAG(1),
	unused1 = FLAG(2),
	unused2 = FLAG(3),
	lock_type_to_env_object = FLAG(4),
	lock_transform_to_env_object = FLAG(5),
	never_placed = FLAG(6),
	lock_name_to_env_object = FLAG(7),
	create_at_rest = FLAG(8),
};

enum e_transform_flags : __int16
{
	mirrored = FLAG(0),
};

enum e_source : __int8
{
	structure = FLAG(0),
	editor = FLAG(1),
	dynamic = FLAG(2),
	legacy = FLAG(3),
};

enum e_bsp_policy : __int8
{
	default = FLAG(0),
	always_placed = FLAG(1),
	manual_bsp_placement = FLAG(2),
};

enum e_active_change_colors : __int32
{
	primary = FLAG(0),
	secondary = FLAG(1),
	tertiary = FLAG(2),
	quaternary = FLAG(3),
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
