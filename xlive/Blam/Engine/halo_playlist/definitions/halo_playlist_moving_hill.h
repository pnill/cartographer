#pragma once
#include "halo_playlist_item_collection.h"

#define k_halo_playlist_hill_move_item_count 10

enum e_halo_playlist_hill_move_time : int32
{
	_halo_playlist_hill_move_time_off,
	_halo_playlist_hill_move_time_thirty_seconds = 30,
	_halo_playlist_hill_move_time_one_minutes = 60,
	_halo_playlist_hill_move_time_two_minutes = 120,
	_halo_playlist_hill_move_time_three_minutes = 180,
	_halo_playlist_hill_move_time_five_minutes = 300,

	k_halo_playlist_hill_move_time_count = 6,
	k_halo_playlist_hill_move_time_invalid = NONE
};

e_halo_playlist_hill_move_time halo_playlist_item_collection_get_hill_move_time_value(wchar_t* value);

bool halo_playlist_item_collection_king_hill_move_time_write_to_variant(wchar_t* value, s_game_variant* variant);
bool halo_playlist_item_collection_headhunter_hill_move_time_write_to_variant(wchar_t* value, s_game_variant* variant);