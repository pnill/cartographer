#pragma once

/* constants */

enum
{
	k_number_of_users = 4,
	k_maximum_players = 16,
	k_player_index_bit_count = 4	// 4 because 4 bits can store 16 players in k_maximum_players
};

enum
{
	k_player_name_length = 16,
	k_shot_entries_per_player = 8,
	k_player_rank_index_count = 64,
};
