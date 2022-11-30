#pragma once
#include "Blam/Cache/DataTypes/BlamPrimitiveType.h"
#include "Blam/Common/Common.h"
#include "Blam/Engine/game/aim_assist.h"
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/simulation/machine_identifier.h"

#pragma pack(push, 1)
enum e_simulation_player_type   //TODO: determine actual names for this enum field
{
	simulation_player_type_local_authority = 0x0,
	simulation_player_type_local_no_authority = 0x1,
	simulation_player_type_local_unk = 0x2,
	simulation_player_type_foreign_synchronous = 0x3,
	simulation_player_type_foreign_no_authority = 0x4,
	simulation_player_type_foreign_distributed = 0x5,
	k_simulation_player_type_count = 0x6,
};

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

struct s_simulation_player
{
	int player_index;
	datum player_datum_index;
	e_simulation_player_type m_player_type;
	XNKID player_identifier;
	s_machine_identifier player_machine_identifier;
	PAD(2);
	void* simulation_world_ptr;
	char field_20[1];
	bool m_active;
	PAD(2);
	unsigned int start_commit_ticks;
	s_player_actions actions;
};
CHECK_STRUCT_SIZE(s_simulation_player, 0x88);
#pragma pack(pop)
