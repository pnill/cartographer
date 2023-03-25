#pragma once
#include "MachineID.h"
#include "Blam\Engine\Players\PlayerActions.h"

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
	player_action actions;
};
CHECK_STRUCT_SIZE(s_simulation_player, 0x88);
