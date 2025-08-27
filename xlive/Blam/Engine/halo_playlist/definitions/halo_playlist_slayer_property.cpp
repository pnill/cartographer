#include "stdafx.h"
#include "halo_playlist_slayer_property.h"

//constexpr wchar_t* g_halo_playlist_slayer_property_strings[k_halo_playlist_slayer_property_item_count]
//{
//	L"Score to Win Round",
//	L"Team Play",
//	L"Team Scoring",
//	L"Team Changing",
//	L"Force Even Teams",
//	L"Bonus Points",
//	L"Suicide Point Loss",
//	L"Death Point Loss"
//};
//
//s_halo_playlist_item g_halo_playlist_slayer_property_items[k_halo_playlist_slayer_property_item_count]
//{
//	{g_halo_playlist_slayer_property_strings[0], _halo_playlist_slayer_property_score_to_win_round},
//	{g_halo_playlist_slayer_property_strings[1], _halo_playlist_slayer_property_team_play},
//	{g_halo_playlist_slayer_property_strings[2], _halo_playlist_slayer_property_team_scoring},
//	{g_halo_playlist_slayer_property_strings[3], _halo_playlist_slayer_property_team_changing},
//	{g_halo_playlist_slayer_property_strings[4], _halo_playlist_slayer_property_force_even_teams},
//	{g_halo_playlist_slayer_property_strings[5], _halo_playlist_slayer_property_bonus_points},
//	{g_halo_playlist_slayer_property_strings[6], _halo_playlist_slayer_property_suicide_point_loss},
//	{g_halo_playlist_slayer_property_strings[7], _halo_playlist_slayer_property_death_point_loss},
//};
//
//s_halo_playlist_item_collection g_halo_playlist_slayer_property_item_collection = { g_halo_playlist_slayer_property_items, k_halo_playlist_slayer_property_item_count };

e_halo_playlist_slayer_property halo_playlist_item_collection_slayer_property_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_slayer_property_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355BD8);
	return (e_halo_playlist_slayer_property)halo_playlist_item_collection_get_value(g_halo_playlist_slayer_property_item_collection, value);
}
