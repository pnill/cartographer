#include "stdafx.h"
#include "halo_playlist_team_property.h"

//constexpr wchar_t* g_halo_playlist_team_property_strings[halo_playlist_team_property_item_count]
//{
//	L"Friendly Fire",
//	L"Respawn Time Modifier",
//	L"Betrayal Penalty"
//};
//
//s_halo_playlist_item g_halo_playlist_team_property_items[halo_playlist_team_property_item_count]
//{
//	{g_halo_playlist_team_property_strings[0], _halo_playlist_team_property_friendly_fire},
//	{g_halo_playlist_team_property_strings[1], _halo_playlist_team_property_respawn_time_modifier},
//	{g_halo_playlist_team_property_strings[2], _halo_playlist_team_property_betrayal_penalty},
//};
//
//s_halo_playlist_item_collection g_halo_playlist_team_property_item_collection = { g_halo_playlist_team_property_items, halo_playlist_team_property_item_count };

e_halo_playlist_team_property halo_playlist_item_collection_team_property_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_team_property_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x3556A0);
	return (e_halo_playlist_team_property)halo_playlist_item_collection_get_value(g_halo_playlist_team_property_item_collection, value);
}
