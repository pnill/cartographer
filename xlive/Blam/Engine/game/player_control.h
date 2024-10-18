#pragma once
#include "game/aim_assist.h"
#include "units/unit_definitions.h"


class c_player_input
{
public:
	real_vector2d throttle;
	real32 trigger;
	real32 secondary_trigger;
	real32 yaw_change;
	real32 pitch_change;
	uint32 field_18;
	uint32 flags_1C;
	uint32 flags_20;
	uint32 flags_24;
	real_euler_angles2d desired_angles;
	s_aim_assist_targeting_result targeting;
	int8 field_50[4];
};

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
	int16 zoom_level;
	s_player_action_weapons weapon_indexes;
	uint16 grenade_index;
	int32 interaction_type;
	int32 interaction_object;
	int32 melee_target_unit;
	s_aim_assist_targeting_result aim_assist_data;
	int32 unk;
};
ASSERT_STRUCT_SIZE(player_action, 0x60);

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
	int8 gap_80[25];
	bool zoom_input_held;
	int16 field_9A;
	int16 zoom_input_updates_held_count;
	int8 gap_9E;
	int8 field_9F;
	int8 gap_A0[12];
	int32 field_AC;
};
ASSERT_STRUCT_SIZE(s_player_control, 0xB0);	

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
};
ASSERT_STRUCT_SIZE(s_player_control_globals, 728);

struct s_player_control_camera_info
{
	datum slave_object;
	int16 parent_seat_index;
	int16 pad;
	s_unit_camera* unit_camera;
	real_point3d location;
};
ASSERT_STRUCT_SIZE(s_player_control_camera_info, 24);

void player_control_disable_local_camera(bool state);

s_player_control_globals* player_control_globals_get(void);
s_player_control* player_control_get(int32 user_index);
int16 player_control_get_zoom_level(int32 user_index);

void player_control_update_dt(real32 dt);

// Gets the FOV from the unit tag of the player with the passed controller index
// We modified it so it overriddes the FOV depending on if the user has a custom FOV set or the forced FOV value is set for custom variants
real32 __cdecl player_control_get_field_of_view(uint32 user_index);

void __cdecl player_control_get_facing_direction(uint32 player_index, real_vector3d* out_facing);

real_euler_angles2d* __cdecl player_control_get_facing(uint32 player_index);

void __cdecl player_control_get_camera_info(uint32 player_index, s_player_control_camera_info* camera_info);

void player_control_apply_patches();
