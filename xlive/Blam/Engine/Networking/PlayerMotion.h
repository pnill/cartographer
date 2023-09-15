#pragma once

#include "Blam/Math/real_math.h"
#include "NetworkPrimitiveTypes.h"

#pragma pack(push, 1)
struct s_player_predicted_aim_assist
{
	float primary_auto_aim_level;
	float secondary_auto_aim_level;
	network_entity_index entity;
	unsigned int model_target;
};
CHECK_STRUCT_SIZE(s_player_predicted_aim_assist, 0x10);

struct s_player_motion
{
	real_euler_angles2d facing;
	real_point2d throttle;
	WORD motion_flags;
	WORD weapon_set_identifier;
	BYTE primary_weapon_index;
	BYTE secondary_weapon_index;
	WORD zoom_level;
	bool primary_predicted_fire_primary;
	bool primary_predicted_fire_secondary;
	bool secondary_predicted_fire_primary;
	bool secondary_predicted_fire_secondary;
	bool predicted_aim_assist_exists;
	BYTE gap_1D[3];
	s_player_predicted_aim_assist aim_info;
	bool weapon_fire_empty_primary;
	bool weapon_fire_empty_secondary;
	bool motion_sensor_velocity_threshold_exceeded;
	BYTE gap33;
};
CHECK_STRUCT_SIZE(s_player_motion, 0x34);
#pragma pack(pop)