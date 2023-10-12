#pragma once

#include "Blam/Engine/Networking/logic/network_session_interface.h"
#include "Blam/Engine/Simulation/game_interface/simulation_game_engine_player.h"
#define k_statborg_entry_total_stat_count 9

// TODO Confirm these are correct
// Confirmed List: 
// statborg_entry_score
enum e_statborg_entry : int
{
	statborg_entry_score = 0,
	statborg_entry_kill = 2,
	statborg_entry_death = 3,
	statborg_entry_suicide = 4,
	statborg_entry_assist = 6
};

struct s_statborg_player_data
{
	short statistic[k_statborg_entry_total_stat_count];
	short multiple_kills;
	short kills_in_a_row;
	short field_16;
	short field_18;
	short field_1A;
	short field_1C;
	short field_1E;
};
CHECK_STRUCT_SIZE(s_statborg_player_data, 32);

// TODO Confirm size on this struct
struct s_statborg_team_data
{
	short statistic[k_statborg_entry_total_stat_count];
};
CHECK_STRUCT_SIZE(s_statborg_team_data, 18);

// TODO reimplement the rest of the member functions for this class
// TODO Confirm size on this struct
class c_game_statborg
{
	bool should_record_stats;
	byte pad_1[3];
	s_statborg_player_data m_player_data[k_maximum_multiplayer_players];
	s_statborg_team_data m_team_data[k_multiplayer_team_count];

public:
	void adjust_player_stat(datum player_datum, e_statborg_entry statistic, short count, int game_result_statistic, bool adjust_team_stat);
	void adjust_team_stat(int team_index, e_statborg_entry statistic, short count, int game_result_statistic);
	short get_player_stat(DWORD player_index, e_statborg_entry statborg_entry);
	short get_team_stat(int team_index, e_statborg_entry statistic);

};
CHECK_STRUCT_SIZE(c_game_statborg, 0x294);

c_game_statborg* game_engine_get_statborg();

void game_statborg_apply_patches();
