#pragma once
#include "halo_playlist_item_collection.h"

bool halo_playlist_item_collection_team_play_write_to_variant(wchar_t* value, s_game_variant* variant);

bool halo_playlist_item_collection_team_changing_write_to_variant(wchar_t* value, s_game_variant* variant);

bool halo_playlist_item_collection_force_even_teams_write_to_variant(wchar_t* value, s_game_variant* variant);

bool halo_playlist_item_collection_king_uncontested_hill_write_to_variant(wchar_t* value, s_game_variant* variant);

bool halo_playlist_item_collection_headhunter_uncontested_hill_write_to_variant(wchar_t* value, s_game_variant* variant);