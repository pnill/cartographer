#pragma once
#include "Blam/Common/Common.h"
#include "Blam/Engine/game/aim_assist.h"
#include "Blam/Math/real_math.h"

enum e_simulation_world_type
{
	_simulation_world_type_none = 0x0,
	_simulation_world_type_local = 0x1,
	_simulation_world_type_synchronous_authority = 0x2,
	_simulation_world_type_synchronous_client = 0x3,
	_simulation_world_type_distributed_authority = 0x4,
	_simulation_world_type_distributed_client = 0x5,
	k_simulation_world_type_count = 0x6,
};

enum e_simulation_world_state
{
	_simulation_world_state_none = 0x0,
	_simulation_world_state_dead = 0x1,
	_simulation_world_state_disconnected = 0x2,
	_simulation_world_state_joining = 0x3,
	_simulation_world_state_active = 0x4,
	_simulation_world_state_handoff = 0x5,
	_simulation_world_state_leaving = 0x6,
	k_simulation_world_state_count = 0x7,
};

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
