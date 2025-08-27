#include "stdafx.h"
#include "halo_playlist_max_heads_carried.h"

constexpr wchar_t* g_halo_playlist_max_heads_carried_strings[k_halo_playlist_max_heads_carried_item_count]
{
	L"Unlimited",
	L"None",
	L"One",
	L"1",
	L"Five",
	L"5",
	L"Ten",
	L"10"
};

s_halo_playlist_item g_halo_playlist_max_heads_carried_items[k_halo_playlist_max_heads_carried_item_count]
{
	{g_halo_playlist_max_heads_carried_strings[0], _headhunter_max_heads_carried_none},
	{g_halo_playlist_max_heads_carried_strings[1], _headhunter_max_heads_carried_none},
	{g_halo_playlist_max_heads_carried_strings[2], _headhunter_max_heads_carried_one},
	{g_halo_playlist_max_heads_carried_strings[3], _headhunter_max_heads_carried_one},
	{g_halo_playlist_max_heads_carried_strings[4], _headhunter_max_heads_carried_five},
	{g_halo_playlist_max_heads_carried_strings[5], _headhunter_max_heads_carried_five},
	{g_halo_playlist_max_heads_carried_strings[6], _headhunter_max_heads_carried_ten},
	{g_halo_playlist_max_heads_carried_strings[7], _headhunter_max_heads_carried_ten}
};

s_halo_playlist_item_collection g_halo_playlist_max_heads_carried_item_collection = { g_halo_playlist_max_heads_carried_items, k_halo_playlist_max_heads_carried_item_count };

e_headhunter_max_heads_carried halo_playlist_item_collection_max_heads_carried_get_value(wchar_t* value)
{
	return (e_headhunter_max_heads_carried)halo_playlist_item_collection_get_value(&g_halo_playlist_max_heads_carried_item_collection, value);
}

bool halo_playlist_item_collection_max_heads_carried_write_to_variant(wchar_t* value, s_game_variant* variant)
{
	e_headhunter_max_heads_carried result = halo_playlist_item_collection_max_heads_carried_get_value(value);

	if (result == k_headhunter_max_heads_carried_invalid)
		return false;

	variant->game_engine_variant.head_hunter.max_heads_carried = result;
	return true;
}

