#include "stdafx.h"
#include "halo_playlist_ctf_property.h"

//constexpr wchar_t* g_halo_playlist_ctf_property_strings[k_halo_playlist_ctf_property_item_count]
//{
//	L"Score to Win Round",
//	L"Team Changing",
//	L"Force Even Teams",
//	L"Flag Type",
//	L"Sudden Death",
//	L"Flag At Home To Score",
//	L"Flag Touch Return",
//	L"Flag Reset Time",
//	L"Slow With Flag",
//	L"Flag Hit Damage",
//	L"Damage Resistance With Flag",
//	L"Active Camo With Flag",
//	L"Vehicle Operation",
//	L"Flag Indicator"
//};
//
//s_halo_playlist_item g_halo_playlist_ctf_property_items[k_halo_playlist_ctf_property_item_count]
//{
//	{g_halo_playlist_ctf_property_strings[0], _halo_playlist_ctf_property_score_to_win_round},
//	{g_halo_playlist_ctf_property_strings[1], _halo_playlist_ctf_property_team_changing},
//	{g_halo_playlist_ctf_property_strings[2], _halo_playlist_ctf_property_force_even_teams},
//	{g_halo_playlist_ctf_property_strings[3], _halo_playlist_ctf_property_flag_type},
//	{g_halo_playlist_ctf_property_strings[4], _halo_playlist_ctf_property_sudden_death},
//	{g_halo_playlist_ctf_property_strings[5], _halo_playlist_ctf_property_flag_at_home_to_score},
//	{g_halo_playlist_ctf_property_strings[6], _halo_playlist_ctf_property_flag_touch_return},
//	{g_halo_playlist_ctf_property_strings[7], _halo_playlist_ctf_property_flag_reset_time},
//	{g_halo_playlist_ctf_property_strings[8], _halo_playlist_ctf_property_slow_with_flag},
//	{g_halo_playlist_ctf_property_strings[9], _halo_playlist_ctf_property_flag_hit_damage},
//	{g_halo_playlist_ctf_property_strings[10], _halo_playlist_ctf_property_damage_resistance_with_flag},
//	{g_halo_playlist_ctf_property_strings[11], _halo_playlist_ctf_property_active_camo_with_flag},
//	{g_halo_playlist_ctf_property_strings[12], _halo_playlist_ctf_property_vehicle_operation},
//	{g_halo_playlist_ctf_property_strings[13], _halo_playlist_ctf_property_flag_indicator}
//};
//
//s_halo_playlist_item_collection g_halo_playlist_ctf_property_item_collection = { g_halo_playlist_ctf_property_items, k_halo_playlist_ctf_property_item_count };

e_halo_playlist_ctf_property halo_playlist_item_collection_ctf_property_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_ctf_property_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x3560C0);
	return (e_halo_playlist_ctf_property)halo_playlist_item_collection_get_value(g_halo_playlist_ctf_property_item_collection, value);
}
