#pragma once
#include "halo_playlist_item_collection.h"

#define k_halo_playlist_header_item_count 5

enum e_halo_playlist_header_type : int32
{
	_halo_playlist_header_playlist = 0,
	_halo_playlist_header_variant = 1,
	_halo_playlist_header_match = 2,

	k_halo_playlist_header_count,

	_halo_playlist_header_none = -1,
};

e_halo_playlist_header_type halo_playlist_item_collection_get_header_type(wchar_t* value);