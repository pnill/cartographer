#include "stdafx.h"
#include "halo_playlist_match_settings_property.h"

//constexpr wchar_t* g_halo_playlist_match_settings_property_strings[k_halo_playlist_match_settings_property_item_count]
//{
//	L"Number of Rounds",
//	L"Round Time Limit",
//	L"Rounds Reset Map",
//	L"Resolve Ties"
//};
//
//s_halo_playlist_item g_halo_playlist_match_settings_property_items[k_halo_playlist_match_settings_property_item_count]
//{
//	{g_halo_playlist_match_settings_property_strings[0], _halo_playlist_match_settings_property_number_of_rounds},
//	{g_halo_playlist_match_settings_property_strings[1], _halo_playlist_match_settings_property_round_time_limit},
//	{g_halo_playlist_match_settings_property_strings[2], _halo_playlist_match_settings_property_rounds_reset_map},
//	{g_halo_playlist_match_settings_property_strings[3], _halo_playlist_match_settings_property_resolve_ties}
//};
//
//s_halo_playlist_item_collection g_halo_playlist_match_settings_property_item_collection = { g_halo_playlist_match_settings_property_items, k_halo_playlist_match_settings_property_item_count };

e_halo_playlist_match_settings_property halo_playlist_item_collection_match_settings_property_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_match_settings_property_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355448);
	return (e_halo_playlist_match_settings_property)halo_playlist_item_collection_get_value(g_halo_playlist_match_settings_property_item_collection, value);
}
