#pragma once

#include "Blam/Engine/tag_files/string_id.h"
#include "Blam/Engine/game/aim_assist.h"


struct s_unit_control_data
{
	string_id animation_state;
	uint16 aiming_speed;
	uint16 weapon_set_identifier;
	uint8 field_8;
	uint8 field_9;
	uint16 grenade_index;
	uint16 zoom_level;
	char gap_E[2];
	int64 control_flags;
	real_vector3d throttle;
	float trigger;
	float secondary_trigger;
	real_vector3d desired_facing;
	real_vector3d desired_aiming;
	real_vector3d desired_looking;
	int32 field_50;
	int32 field_54;
	int32 field_58;
	s_aim_assist_targeting_result target_info;

};
CHECK_STRUCT_SIZE(s_unit_control_data, 0x80);