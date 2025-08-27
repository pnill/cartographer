#include "stdafx.h"
#include "halo_playlist_territories_property.h"

//constexpr wchar_t* g_halo_playlist_territories_property_strings[k_halo_playlist_territories_property_item_count]
//{
//	L"Score to Win Round",
//	L"Team Changing",
//	L"Force Even Teams",
//	L"Territory Count",
//	L"Contest Time",
//	L"Control Time"
//};
//
//s_halo_playlist_item g_halo_playlist_territories_property_items[k_halo_playlist_territories_property_item_count]
//{
//	{g_halo_playlist_territories_property_strings[0], _halo_playlist_territories_property_score_to_win_round},
//	{g_halo_playlist_territories_property_strings[1], _halo_playlist_territories_property_team_changing},
//	{g_halo_playlist_territories_property_strings[2], _halo_playlist_territories_property_force_even_teams},
//	{g_halo_playlist_territories_property_strings[3], _halo_playlist_territories_property_territory_count},
//	{g_halo_playlist_territories_property_strings[4], _halo_playlist_territories_property_contest_time},
//	{g_halo_playlist_territories_property_strings[5], _halo_playlist_territories_property_control_time}
//};
//
//s_halo_playlist_item_collection g_halo_playlist_territories_property_item_collection = { g_halo_playlist_territories_property_items, k_halo_playlist_territories_property_item_count };

e_halo_playlist_territories_property halo_playlist_item_collection_territories_property_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_territories_property_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x356468);
	return (e_halo_playlist_territories_property)halo_playlist_item_collection_get_value(g_halo_playlist_territories_property_item_collection, value);
}
