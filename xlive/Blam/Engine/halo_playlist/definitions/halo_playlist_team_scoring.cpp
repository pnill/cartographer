#include "stdafx.h"
#include "halo_playlist_team_scoring.h"

//constexpr wchar_t* g_halo_playlist_team_scoring_strings[k_halo_playlist_team_scoring_item_count]
//{
//	L"Sum",
//	L"Minimum",
//	L"Maximum"
//};
//
//s_halo_playlist_item g_halo_playlist_team_scoring_items[k_halo_playlist_team_scoring_item_count]
//{
//	{g_halo_playlist_team_scoring_strings[0], _game_engine_team_score_sum},
//	{g_halo_playlist_team_scoring_strings[1], _game_engine_team_score_min},
//	{g_halo_playlist_team_scoring_strings[2], _game_engine_team_score_max}
//};
//
//s_halo_playlist_item_collection g_halo_playlist_team_scoring_item_collection = { g_halo_playlist_team_scoring_items, k_halo_playlist_team_scoring_item_count };

e_game_engine_team_score halo_playlist_item_collection_team_scoring_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_team_scoring_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x3555F0);
	return (e_game_engine_team_score)halo_playlist_item_collection_get_value(g_halo_playlist_team_scoring_item_collection, value);
}


bool halo_playlist_item_collection_team_scoring_write_to_variant(wchar_t* value, s_game_variant* variant)
{
	e_game_engine_team_score result = halo_playlist_item_collection_team_scoring_get_value(value);

	if (result == k_game_engine_team_score_invalid)
		return false;

	variant->team_score_setting = result;
	return true;
}