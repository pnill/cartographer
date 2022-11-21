#pragma once
#include "PlayerActions.h"

struct s_player_control
{
	datum slave_object;
	int control_flag;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	s_player_actions actions;
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
