#pragma once

#include "machine_id.h"
#include "Blam/Engine/game/player_control.h"

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

#pragma pack(push, 1)
struct c_simulation_player
{
	int32 m_player_index;
	datum m_player_datum_index;
	e_simulation_player_type m_player_type;
	uint64 m_player_identifier;
	s_machine_identifier m_player_machine_identifier;
	PAD(2);
	void* m_simulation_world;
	char m_field_20;
	bool m_active;
	PAD(2);
	uint32 m_start_commit_ticks;
	player_action m_actions;
};
#pragma pack(pop)
CHECK_STRUCT_SIZE(c_simulation_player, 0x88);
