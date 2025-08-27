#include "stdafx.h"
#include "halo_playlist_player_speed.h"

constexpr wchar_t* g_halo_playlist_player_speed_strings[k_halo_playlist_player_speed_item_count]
{
	L"Slow",
	L"Normal",
	L"Fast"
};

s_halo_playlist_item g_halo_playlist_headhunter_player_speed_items[k_halo_playlist_player_speed_item_count]
{
	{g_halo_playlist_player_speed_strings[0], _ctf_engine_player_speed_slow},
	{g_halo_playlist_player_speed_strings[1], _ctf_engine_player_speed_normal},
	{g_halo_playlist_player_speed_strings[2], _ctf_engine_player_speed_fast}
};

s_halo_playlist_item_collection g_halo_playlist_headhunter_player_speed_item_collection{g_halo_playlist_headhunter_player_speed_items,  k_halo_playlist_player_speed_item_count};

e_ctf_engine_player_speed halo_playlist_item_collection_headhunter_speed_with_heads_get_value(wchar_t* value)
{
	return (e_ctf_engine_player_speed)halo_playlist_item_collection_get_value(&g_halo_playlist_headhunter_player_speed_item_collection, value);
}

bool halo_playlist_item_collection_headhunter_speed_with_heads_write_to_variant(wchar_t* value, s_game_variant* variant)
{
	e_ctf_engine_player_speed result = (e_ctf_engine_player_speed)halo_playlist_item_collection_headhunter_speed_with_heads_get_value(value);

	if (result == k_ctf_engine_players_speed_invalid)
		return false;

	variant->game_engine_variant.head_hunter.speed_with_heads = result;
	return true;
}
