#pragma once
#include "object_definition.h"
#include "object_scenario_definitions.h"

#include "objects.h"

/* constants */


enum
{
	SCENERY_DEFINITION_TAG = 'scen',
	SCENERY_DEFINITION_VERSION = 1,		// TODO: verify
};

/* macros */

#define scenery_definition_get(index)	((struct scenery_definition*)tag_get(SCENERY_DEFINITION_TAG, (index)))

/* enums */

enum e_scenery_pathfinding_policy : int16
{
	_scenery_pathfinding_policy_dynamic = 0,
	_scenery_pathfinding_policy_cutout = 1,
	_scenery_pathfinding_policy_static = 2,
	_scenery_pathfinding_policy_none = 3
};

enum e_scenery_definition_flags : uint16
{
	_scenery_definition_physically_simulates_bit = 0,	// stimulates
	k_scenery_definition_flag_count
};

enum e_scenery_lightmapping_policy : int16
{
	_scenery_lightmapping_per_vertex = 0,
	_scenery_lightmapping_per_pixel = 1,		// not implemented
	_scenery_lightmapping_dynamic = 2,
	k_scenery_lightmapping_count,
	_scenery_lightmapping_policy_none = NONE
};

enum e_scenery_valid_multiplayer_games : int16
{
	_scenery_valid_multiplayer_game_capture_the_flag_bit = 0,
	_scenery_valid_multiplayer_game_slayer_bit = 1,
	_scenery_valid_multiplayer_game_oddball_bit = 2,
	_scenery_valid_multiplayer_game_king_of_the_hill_bit = 3,
	_scenery_valid_multiplayer_game_juggernaut_bit = 4,
	_scenery_valid_multiplayer_game_territories_bit = 5,
	_scenery_valid_multiplayer_game_assault_bit = 6,
};

/* structures */

struct _scenery_definition
{
	// Explaination("Pathfinding", "Indicate whether, by default, we should create pathfinding data for this type of scenery")

	e_scenery_pathfinding_policy pathfinding_policy;
	c_flags_no_init<e_scenery_definition_flags, uint16, k_scenery_definition_flag_count> flags;

	// Explaination("Lightmapping", "Indicate whether, by default, how we should lightmap this type of scenery")

	e_scenery_lightmapping_policy lightmapping_policy;
	int16 pad;
};
ASSERT_STRUCT_SIZE(_scenery_definition, 8);

struct scenery_definition
{
	_object_definition object;
	_scenery_definition scenery;
};
ASSERT_STRUCT_SIZE(scenery_definition, 196);

struct _scenery_datum
{
	uint32 flags;
	int16 pathfinding_references_index;
	e_scenery_pathfinding_policy pathfinding_policy;
	int32 lightmapping_policy;	// e_scenery_lightmapping_policy
	int32 animation_relative_object_index;
};

struct scenery_datum
{
	datum definition_index;
	_object_datum object;
	_scenery_datum scenery;
};
ASSERT_STRUCT_SIZE(scenery_datum, 316)

struct s_scenario_scenery_datum
{
	e_scenery_pathfinding_policy pathfinding_policy;
	e_scenery_lightmapping_policy lightmapping_policy;
	s_tag_block pathfinding_references;	// struct: pathfinding_object_index_list
	int16 pad;
	e_scenery_valid_multiplayer_games valid_multiplayer_games;
};
ASSERT_STRUCT_SIZE(s_scenario_scenery_datum, 16);

// max count: MAXIMUM_SCENERY_DATUMS_PER_SCENARIO 2000
struct s_scenario_scenery
{
	// filt
	// Block index: scenario_scenery_palette
	int16 type;
	// filt
	// Block index: scenario_object_name
	int16 name;

	s_scenario_object_datum object_data;
	s_scenario_object_permutation permutation_data;
	s_scenario_scenery_datum scenery_data;
};
ASSERT_STRUCT_SIZE(s_scenario_scenery, 92);
