#pragma once
#include "damage.h"
#include "emblems.h"
#include "object_identifier.h"
#include "object_location.h"
#include "object_scenario_definitions.h"

#include "math/color_math.h"

/* structures */

struct object_placement_data
{
	datum definition_index;
	c_object_identifier object_identifier;
	string_id variant_name;
	uint32 scenario_datum_index;
	e_bsp_policy placement_policy;
	char pad1[3];
	
	uint32 flags;
	real_point3d position;
	real_vector3d forward;
	real_vector3d up;
	real_vector3d translational_velocity;
	real_vector3d angular_velocity;
	real32 scale;

	int32 creator_player_index;
	datum creator_object_index;
	int32 creator_team_index;
	s_damage_owner damage_owner;
	uint32 change_color_override_mask;
	real_rgb_color change_color_overrides[4];
	s_emblem_info emblem_info;
	int8 pad2;
	int32 region_index;
	uint16 destroyed_constraints_flag;
	uint16 loosened_constraints_flag;
	int16 ai_state_type;
	int16 ai_state_size;
	bool location_valid;
	uint8 unk_22;
	uint16 unk_23;
	s_location location;
};
ASSERT_STRUCT_SIZE(object_placement_data, 0xC4);
