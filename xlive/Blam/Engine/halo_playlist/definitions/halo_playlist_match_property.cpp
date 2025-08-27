#include "stdafx.h"
#include "halo_playlist_match_property.h"

//constexpr wchar_t* g_halo_playlist_match_property_strings[k_halo_playlist_match_property_count]
//{
//	L"Variant",
//	L"Map",
//	L"Weight",
//	L"Minimum Players",
//	L"Maximum Players"
//};
//
//s_halo_playlist_item g_halo_playlist_match_property_items[k_halo_playlist_match_property_count]
//{
//	{g_halo_playlist_match_property_strings[0], _halo_playlist_match_property_type_variant},
//	{g_halo_playlist_match_property_strings[1] ,_halo_playlist_match_property_type_map},
//	{g_halo_playlist_match_property_strings[2], _halo_playlist_match_property_type_weight},
//	{g_halo_playlist_match_property_strings[3], _halo_playlist_match_property_type_minimum_players},
//	{g_halo_playlist_match_property_strings[4], _halo_playlist_match_property_type_maximum_players}
//};
//
//s_halo_playlist_item_collection g_halo_playlist_match_property_collection = { g_halo_playlist_match_property_items, k_halo_playlist_match_property_count };

e_halo_playlist_match_property_type halo_playlist_item_collection_get_match_property_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_match_property_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355260);
	return INVOKE_TYPE(0, 0xF3BD, e_halo_playlist_match_property_type(*)(s_halo_playlist_item_collection*, wchar_t*), g_halo_playlist_match_property_collection, value);
}

wchar_t* halo_playlist_item_collection_match_property_get_name(e_halo_playlist_match_property_type value)
{
	ASSERT(IN_RANGE(value, -1, k_halo_playlist_match_property_type_count));

	s_halo_playlist_item_collection* g_halo_playlist_match_property_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355260);

	return halo_playlist_item_collection_get_name(g_halo_playlist_match_property_collection, value);
}
