#pragma once
#include "Blam\Cache\DataTypes\BlamPrimitiveType.h"
#include "Blam\Common\Common.h"
#include "Blam/Engine/math/real_math.h"

#pragma pack(push, 1)
struct s_aim_assist_targetting_data
{
	datum target_object;
	datum model_target;
	datum target_player;
	char gap_C[12];
	signed __int16 auto_aim_flags;
	char gap_1A[2];
	float primary_auto_aim_level;
	float secondary_auto_aim_level;
};
CHECK_STRUCT_SIZE(s_aim_assist_targetting_data, 0x24);

struct s_player_actions
{
	int control_flag0;
	int control_flag1;
	real_euler_angles2d facing;
	real_point2d throttle;
	float trigger;
	float secondary_trigger;
	DWORD action_flags;
	WORD weapon_set_identifier;
	BYTE primary_weapon_index;
	BYTE secondary_weapon_index;
	WORD grenade_index;
	WORD zoom_level;
	int interaction_type;
	int interaction_object;
	int melee_target_unit;
	s_aim_assist_targetting_data aim_assist_data;
	int unk;
};
CHECK_STRUCT_SIZE(s_player_actions, 0x60);
#pragma pack(pop)
