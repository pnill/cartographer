#pragma once
#include "Blam/Common/Common.h"
#include "Blam/Engine/game/aim_assist.h"
#include "Blam/Math/real_math.h"

#pragma pack(push, 1)
struct player_action
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
CHECK_STRUCT_SIZE(player_action, 0x60);
#pragma pack(pop)

struct s_player_control
{
	datum slave_object;
	int control_flag;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	player_action actions;
	char gap_78[4];
	DWORD action_context;
	char gap_80[31];
	char field_9F;
	char gap_A0[12];
	int field_AC;
};
CHECK_STRUCT_SIZE(s_player_control, 0xB0);	

struct s_player_control_globals
{
	char initialized;
	char Pad1[3];
	int field_4;
	int field_8;
	int field_C;
	bool disableCamera;
	PAD(7);
	s_player_control local_players[4];
};
CHECK_STRUCT_SIZE(s_player_control_globals, 0x2D8);
