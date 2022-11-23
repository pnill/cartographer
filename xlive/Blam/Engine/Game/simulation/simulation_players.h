#pragma once
#include "Blam/Cache/DataTypes/BlamPrimitiveType.h"
#include "Blam/Common/Common.h"
#include "Blam/Engine/Game/game/aim_assist.h"
#include "Blam/Engine/Game/math/real_math.h"

#pragma pack(push, 1)
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
