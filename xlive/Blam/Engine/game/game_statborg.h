#pragma once
#include "Blam/Common/Common.h"

struct statborg_player_data
{
	int field_0;
	short score[2];
	char player_data[24];
};
CHECK_STRUCT_SIZE(statborg_player_data, 32);

class c_game_statborg
{
	statborg_player_data player_data[16];

public:
	short get_player_score_by_type(DWORD player_index, byte score_type);
};
CHECK_STRUCT_SIZE(c_game_statborg, 512);

c_game_statborg* game_engine_get_statborg();
