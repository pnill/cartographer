#include "stdafx.h"
#include "halo_playlist_playlist_property.h"

//constexpr wchar_t* g_halo_playlist_playlist_property_strings[k_halo_playlist_playlist_property_item_count]
//{
//	L"Shuffle",
//	L"Pregame Team Selection Delay",
//	L"Team Selection Delay",
//	L"Pregame Delay",
//	L"Postgame Delay"
//};
//
//s_halo_playlist_item g_halo_playlist_playlist_property_items[k_halo_playlist_playlist_property_item_count]
//{
//	{g_halo_playlist_playlist_property_strings[0], _halo_playlist_playlist_property_shuffle},
//	{g_halo_playlist_playlist_property_strings[1], _halo_playlist_playlist_property_pregame_team_selection_delay},
//	{g_halo_playlist_playlist_property_strings[2], _halo_playlist_playlist_property_team_selection_delay},
//	{g_halo_playlist_playlist_property_strings[3], _halo_playlist_playlist_property_pregame_delay},
//	{g_halo_playlist_playlist_property_strings[4], _halo_playlist_playlist_property_postgame_delay},
//};
//
//s_halo_playlist_item_collection g_halo_playlist_playlist_property_item_collection = { g_halo_playlist_playlist_property_items, k_halo_playlist_playlist_property_item_count };

e_halo_playlist_playlist_property_type halo_playlist_item_collection_playlist_property_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_playlist_property_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x3552C0);
	return (e_halo_playlist_playlist_property_type)halo_playlist_item_collection_get_value(g_halo_playlist_playlist_property_item_collection, value);
}
