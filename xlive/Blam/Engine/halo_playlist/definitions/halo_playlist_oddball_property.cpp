#include "stdafx.h"
#include "halo_playlist_oddball_property.h"

//constexpr wchar_t* g_halo_playlist_oddball_property_strings[k_halo_playlist_oddball_property_item_count]
//{
//	L"Score to Win Round",
//	L"Team Play",
//	L"Team Scoring",
//	L"Team Changing",
//	L"Force Even Teams",
//	L"Ball Count",
//	L"Ball Hit Damage",
//	L"Speed With Ball",
//	L"Toughness With Ball",
//	L"Active Camo With Ball",
//	L"Vehicle Operation",
//	L"Ball Indicator"
//};
//
//s_halo_playlist_item g_halo_playlist_oddball_property_items[k_halo_playlist_oddball_property_item_count]
//{
//	{g_halo_playlist_oddball_property_strings[0], _halo_playlist_oddball_property_score_to_win_round},
//	{g_halo_playlist_oddball_property_strings[1], _halo_playlist_oddball_property_team_play},
//	{g_halo_playlist_oddball_property_strings[2], _halo_playlist_oddball_property_team_scoring},
//	{g_halo_playlist_oddball_property_strings[3], _halo_playlist_oddball_property_team_changing},
//	{g_halo_playlist_oddball_property_strings[4], _halo_playlist_oddball_property_force_even_teams},
//	{g_halo_playlist_oddball_property_strings[5], _halo_playlist_oddball_property_ball_count},
//	{g_halo_playlist_oddball_property_strings[6], _halo_playlist_oddball_property_ball_hit_damage},
//	{g_halo_playlist_oddball_property_strings[7], _halo_playlist_oddball_property_speed_with_ball},
//	{g_halo_playlist_oddball_property_strings[8], _halo_playlist_oddball_property_toughness_with_ball},
//	{g_halo_playlist_oddball_property_strings[9], _halo_playlist_oddball_property_active_camo_with_ball},
//	{g_halo_playlist_oddball_property_strings[10], _halo_playlist_oddball_property_vehicle_operation},
//	{g_halo_playlist_oddball_property_strings[11], _halo_playlist_oddball_property_ball_indicator}
//};
//
//s_halo_playlist_item_collection g_halo_playlist_oddball_property_item_collection = { g_halo_playlist_oddball_property_items, k_halo_playlist_oddball_property_item_count };

e_halo_playlist_oddball_property halo_playlist_item_collection_oddball_property_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_oddball_property_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355E60);
	return (e_halo_playlist_oddball_property)halo_playlist_item_collection_get_value(g_halo_playlist_oddball_property_item_collection, value);
}
