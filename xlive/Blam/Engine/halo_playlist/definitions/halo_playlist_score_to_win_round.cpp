#include "stdafx.h"
#include "halo_playlist_score_to_win_round.h"

//constexpr wchar_t* g_halo_playlist_score_to_win_round_strings[k_halo_playlist_score_to_win_round_item_count]
//{
//	L"Unlimited",
//	L"1",
//	L"3",
//	L"5",
//	L"10",
//	L"15",
//	L"20",
//	L"25",
//	L"50",
//	L"100",
//	L"250"
//};
//
//constexpr wchar_t* g_halo_playlist_score_to_win_round_time_strings[k_halo_playlist_score_to_win_round_time_item_count]
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
//	L"7 Minutes",
//	L"7",
//	L"10 Minutes",
//	L"10",
//	L"15 Minutes",
//	L"15"
//};
//
//s_halo_playlist_item g_halo_playlist_score_to_win_round_items[k_halo_playlist_score_to_win_round_item_count]
//{
//	{g_halo_playlist_score_to_win_round_strings[0], _halo_playlist_score_to_win_round_unlimited },
//	{g_halo_playlist_score_to_win_round_strings[1], _halo_playlist_score_to_win_round_one},
//	{g_halo_playlist_score_to_win_round_strings[2], _halo_playlist_score_to_win_round_three},
//	{g_halo_playlist_score_to_win_round_strings[3], _halo_playlist_score_to_win_round_five},
//	{g_halo_playlist_score_to_win_round_strings[4], _halo_playlist_score_to_win_round_ten},
//	{g_halo_playlist_score_to_win_round_strings[5], _halo_playlist_score_to_win_round_fifteen},
//	{g_halo_playlist_score_to_win_round_strings[6], _halo_playlist_score_to_win_round_twenty},
//	{g_halo_playlist_score_to_win_round_strings[7], _halo_playlist_score_to_win_round_twenty_five},
//	{g_halo_playlist_score_to_win_round_strings[8], _halo_playlist_score_to_win_round_fifty},
//	{g_halo_playlist_score_to_win_round_strings[9], _halo_playlist_score_to_win_round_one_hundred},
//	{g_halo_playlist_score_to_win_round_strings[10], _halo_playlist_score_to_win_round_two_hundred_fifty}
//};
//
//s_halo_playlist_item g_halo_playlist_score_to_win_round_time_items[k_halo_playlist_score_to_win_round_time_item_count]
//{
//	{g_halo_playlist_score_to_win_round_time_strings[0], _halo_playlist_score_to_win_round_time_off},
//	{g_halo_playlist_score_to_win_round_time_strings[1], _halo_playlist_score_to_win_round_time_thirty_seconds},
//	{g_halo_playlist_score_to_win_round_time_strings[2], _halo_playlist_score_to_win_round_time_one_minutes},
//	{g_halo_playlist_score_to_win_round_time_strings[3], _halo_playlist_score_to_win_round_time_one_minutes},
//	{g_halo_playlist_score_to_win_round_time_strings[4], _halo_playlist_score_to_win_round_time_two_minutes},
//	{g_halo_playlist_score_to_win_round_time_strings[5], _halo_playlist_score_to_win_round_time_two_minutes},
//	{g_halo_playlist_score_to_win_round_time_strings[6], _halo_playlist_score_to_win_round_time_three_minutes},
//	{g_halo_playlist_score_to_win_round_time_strings[7], _halo_playlist_score_to_win_round_time_three_minutes},
//	{g_halo_playlist_score_to_win_round_time_strings[8], _halo_playlist_score_to_win_round_time_five_minutes},
//	{g_halo_playlist_score_to_win_round_time_strings[9], _halo_playlist_score_to_win_round_time_five_minutes},
//	{g_halo_playlist_score_to_win_round_time_strings[10], _halo_playlist_score_to_win_round_time_seven_minutes},
//	{g_halo_playlist_score_to_win_round_time_strings[11], _halo_playlist_score_to_win_round_time_seven_minutes},
//	{g_halo_playlist_score_to_win_round_time_strings[12], _halo_playlist_score_to_win_round_time_ten_minutes},
//	{g_halo_playlist_score_to_win_round_time_strings[13], _halo_playlist_score_to_win_round_time_ten_minutes},
//	{g_halo_playlist_score_to_win_round_time_strings[14], _halo_playlist_score_to_win_round_time_fifteen_minutes},
//	{g_halo_playlist_score_to_win_round_time_strings[15], _halo_playlist_score_to_win_round_time_fifteen_minutes},
//};
//
//s_halo_playlist_item_collection g_halo_playlist_score_to_win_round_collection = { g_halo_playlist_score_to_win_round_items, k_halo_playlist_score_to_win_round_item_count };
//
//s_halo_playlist_item_collection g_halo_playlist_score_to_win_round_time_collection = { g_halo_playlist_score_to_win_round_time_items, k_halo_playlist_score_to_win_round_time_item_count };



e_halo_playlist_score_to_win_round halo_playlist_item_collection_get_score_to_win_round_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_score_to_win_round_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355B90);
	return (e_halo_playlist_score_to_win_round)halo_playlist_item_collection_get_value(g_halo_playlist_score_to_win_round_collection, value);
}

e_halo_playlist_score_to_win_round_time halo_playlist_item_collection_get_score_to_win_round_time_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_score_to_win_round_time_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355D70);
	return (e_halo_playlist_score_to_win_round_time)halo_playlist_item_collection_get_value(g_halo_playlist_score_to_win_round_time_collection, value);
}

bool halo_playlist_item_collection_score_to_win_round_write_to_variant(wchar_t* value, s_game_variant* variant)
{
	e_halo_playlist_score_to_win_round res = halo_playlist_item_collection_get_score_to_win_round_value(value);

	if (res == k_halo_playlist_score_to_win_round_invalid)
		return false;

	variant->score_to_win_round = res;
	return true;
}

bool halo_playlist_item_collection_score_to_win_round_time_write_to_variant(wchar_t* value, s_game_variant* variant)
{
	e_halo_playlist_score_to_win_round_time res = halo_playlist_item_collection_get_score_to_win_round_time_value(value);

	if (res == k_halo_playlist_score_to_win_round_time_invalid)
		return false;

	variant->score_to_win_round = res;
	return true;
}
