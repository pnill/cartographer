#pragma once
#include "halo_playlist_item_collection.h"

#define k_halo_playlist_team_scoring_item_count 3

e_game_engine_team_score halo_playlist_item_collection_team_scoring_get_value(wchar_t* value);
bool halo_playlist_item_collection_team_scoring_write_to_variant(wchar_t* value, s_game_variant* variant);