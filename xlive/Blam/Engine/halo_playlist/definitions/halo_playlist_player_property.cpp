#include "stdafx.h"
#include "halo_playlist_player_property.h"

//constexpr wchar_t* g_halo_playlist_player_property_strings[k_halo_playlist_player_property_item_count]
//{
//	L"Max Active Players",
//	L"Lives Per Round",
//	L"Respawn Time",
//	L"Suicide Penalty",
//	L"Shield Type",
//	L"Motion Sensor",
//	L"Active Camo",
//	L"Extra Damage",
//	L"Damage Resistance"
//};
//
//s_halo_playlist_item g_halo_playlist_player_property_items[k_halo_playlist_player_property_item_count]
//{
//	{g_halo_playlist_player_property_strings[0], _halo_playlist_player_property_max_active_players},
//	{g_halo_playlist_player_property_strings[1], _halo_playlist_player_property_lives_per_round},
//	{g_halo_playlist_player_property_strings[2], _halo_playlist_player_property_respawn_time},
//	{g_halo_playlist_player_property_strings[3], _halo_playlist_player_property_suicide_penalty},
//	{g_halo_playlist_player_property_strings[4], _halo_playlist_player_property_shield_type},
//	{g_halo_playlist_player_property_strings[5], _halo_playlist_player_property_motion_sensor},
//	{g_halo_playlist_player_property_strings[6], _halo_playlist_player_property_active_camo},
//	{g_halo_playlist_player_property_strings[7], _halo_playlist_player_property_extra_damage},
//	{g_halo_playlist_player_property_strings[8], _halo_playlist_player_property_damage_resistance}
//};
//
//s_halo_playlist_item_collection g_halo_playlist_player_property_item_collection = { g_halo_playlist_player_property_items, k_halo_playlist_player_property_item_count };

e_halo_playlist_player_property halo_playlist_item_collection_player_property_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_player_property_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x3555D0);
	return (e_halo_playlist_player_property)halo_playlist_item_collection_get_value(g_halo_playlist_player_property_item_collection, value);
}
