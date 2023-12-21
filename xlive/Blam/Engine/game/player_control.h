#pragma once

#include "Blam/Engine/game/aim_assist.h"
#include "Blam/Engine/math/real_math.h"

struct s_player_action_weapons
{
	int8 primary_weapon_index;
	int8 secondary_weapon_index;
};

struct player_action
{
	real32 control_flag0;
	real32 control_flag1;
	real_euler_angles2d facing;
	real_point2d throttle;
	real32 trigger;
	real32 secondary_trigger;
	uint32 action_flags;
	uint16 weapon_set_identifier;
	s_player_action_weapons weapon_indexes;
	uint16 grenade_index;
	uint16 zoom_level;
	int32 interaction_type;
	int32 interaction_object;
	int32 melee_target_unit;
	s_aim_assist_targetting_data aim_assist_data;
	int32 unk;
};
CHECK_STRUCT_SIZE(player_action, 0x60);

struct s_player_control
{
	datum unit_datum_index;
	int32 control_flag;
	int32 field_8;
	int32 field_C;
	int32 field_10;
	int32 field_14;
	player_action actions;
	int8 gap_78[4];
	uint32 action_context;
	int8 gap_80[31];
	int8 field_9F;
	int8 gap_A0[12];
	int32 field_AC;
};
CHECK_STRUCT_SIZE(s_player_control, 0xB0);	

struct s_player_control_globals
{
	bool initialized;
	int8 pad_1[3];
	int32 field_4;
	int32 field_8;
	int32 field_C;
	bool disable_camera;
	int8 pad[7];
	s_player_control local_players[4];

	static s_player_control_globals* get();
};
CHECK_STRUCT_SIZE(s_player_control_globals, 0x2D8);
