#pragma once
#include "saved_games/game_variant.h"

struct s_halo_playlist_item
{
	wchar_t* name;
	int32 value;
};
ASSERT_STRUCT_SIZE(s_halo_playlist_item, 8);

struct s_halo_playlist_item_collection
{
	s_halo_playlist_item* items;
	uint32 count;
};
ASSERT_STRUCT_SIZE(s_halo_playlist_item_collection, 8);

wchar_t* halo_playlist_item_collection_get_name(s_halo_playlist_item_collection* collection, int32 value);

int32 halo_playlist_item_collection_get_value(s_halo_playlist_item_collection* collection, wchar_t* value);

bool halo_playlist_item_collection_get_boolean_value(wchar_t* value, bool* out_result);

// range 1-600 allows second and minute specifier
// used for score, respawn times, reset times, etc
int32 halo_playlist_item_collection_get_int_time_value(wchar_t* value);

// range 1-1000
int32 halo_playlist_item_collection_get_int_value(wchar_t* value);

// range 1-16
int16 halo_playlist_item_collection_player_count_get_value(wchar_t* value);