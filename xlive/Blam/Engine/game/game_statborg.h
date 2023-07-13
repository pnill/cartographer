#pragma once
#include "Blam/Common/Common.h"
#include "Blam/Engine/Simulation/GameInterface/simulation_game_engine_player.h"
#define k_statborg_entry_total_stat_count 9

enum e_statborg_entry
{
	statborg_entry_score = 0
};

struct s_statborg_player_data
{
	short score[k_statborg_entry_total_stat_count];
	short multiple_kills;
	short kills_in_a_row;
	short field_16;
	short field_18;
	short field_1A;
	short field_1C;
	short field_1E;
};
CHECK_STRUCT_SIZE(s_statborg_player_data, 32);

struct s_statborg_team_data
{
	short score[k_statborg_entry_total_stat_count];
};
CHECK_STRUCT_SIZE(s_statborg_team_data, 18);

// TODO reimplement the rest of the member functions for this class
class c_game_statborg
{
	bool m_record_stats;
	byte pad_1[3];
	s_statborg_player_data m_player_data[k_maximum_multiplayer_players];
	s_statborg_team_data m_team_data[8];

public:
	short get_player_score_by_type(DWORD player_index, byte score_type);
};
CHECK_STRUCT_SIZE(c_game_statborg, 0x294);

c_game_statborg* game_engine_get_statborg();
