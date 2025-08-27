#include "stdafx.h"
#include "halo_playlist_moving_hill.h"

//constexpr wchar_t* g_halo_playlist_hill_move_time_strings[k_halo_playlist_hill_move_item_count]
//{
//	L"Off",
//	L"30 Seconds",
//	L"1 Minute",
//	L"1",
//	L"2 Minutes",
//	L"2",
//	L"3 Minutes",
//	L"3",
//	L"5 Minutes",
//	L"5",
//};
//
//s_halo_playlist_item g_halo_playlist_hill_move_time_items[k_halo_playlist_hill_move_item_count]
//{
//	{g_halo_playlist_hill_move_time_strings[0], _halo_playlist_hill_move_time_off},
//	{g_halo_playlist_hill_move_time_strings[1], _halo_playlist_hill_move_time_thirty_seconds},
//	{g_halo_playlist_hill_move_time_strings[2], _halo_playlist_hill_move_time_one_minutes},
//	{g_halo_playlist_hill_move_time_strings[3], _halo_playlist_hill_move_time_one_minutes},
//	{g_halo_playlist_hill_move_time_strings[4], _halo_playlist_hill_move_time_two_minutes},
//	{g_halo_playlist_hill_move_time_strings[5], _halo_playlist_hill_move_time_two_minutes},
//	{g_halo_playlist_hill_move_time_strings[6], _halo_playlist_hill_move_time_three_minutes},
//	{g_halo_playlist_hill_move_time_strings[7], _halo_playlist_hill_move_time_three_minutes},
//	{g_halo_playlist_hill_move_time_strings[8], _halo_playlist_hill_move_time_five_minutes},
//	{g_halo_playlist_hill_move_time_strings[9], _halo_playlist_hill_move_time_five_minutes},
//};
//
//s_halo_playlist_item_collection g_halo_playlist_hill_move_time_item_collection = { g_halo_playlist_hill_move_time_items, k_halo_playlist_hill_move_item_count };

e_halo_playlist_hill_move_time halo_playlist_item_collection_get_hill_move_time_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_hill_move_time_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355C88);
	return (e_halo_playlist_hill_move_time)halo_playlist_item_collection_get_value(g_halo_playlist_hill_move_time_item_collection, value);
}

bool halo_playlist_item_collection_king_hill_move_time_write_to_variant(wchar_t* value, s_game_variant* variant)
{
	e_halo_playlist_hill_move_time result = halo_playlist_item_collection_get_hill_move_time_value(value);

	if (result == k_halo_playlist_hill_move_time_invalid)
		return false;

	variant->game_engine_variant.king.hill_move_time = (uint16)result;
	return true;
}

bool halo_playlist_item_collection_headhunter_hill_move_time_write_to_variant(wchar_t* value, s_game_variant* variant)
{
	e_halo_playlist_hill_move_time result = halo_playlist_item_collection_get_hill_move_time_value(value);

	if (result == k_halo_playlist_hill_move_time_invalid)
		return false;

	variant->game_engine_variant.head_hunter.hill_move_time = (uint16)result;
	return true;
}


