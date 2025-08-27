#pragma once
#include "halo_playlist_item_collection.h"
#include "saved_games/game_variant.h"

#define k_halo_playlist_game_type_item_count 11

wchar_t* halo_playlist_item_collection_game_type_get_name(e_game_variant_description_index value);
e_game_variant_description_index halo_playlist_item_collection_game_type_get_value(wchar_t* value);