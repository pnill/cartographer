#include "stdafx.h"
#include "halo_playlist_assault_property.h"

//constexpr wchar_t* g_halo_playlist_assault_property_strings[k_halo_playlist_assault_property_item_count]
//{
//	L"Score to Win Round",
//	L"Team Changing",
//	L"Force Even Teams",
//	L"Bomb Type",
//	L"Enemy Bomb Indicator",
//	L"Sudden Death",
//	L"Bomb Touch Return",
//	L"Bomb Reset Time",
//	L"Bomb Arm Time",
//	L"Sticky Arming",
//	L"Slow With Bomb",
//	L"Bomb Hit Damage",
//	L"Damage Resistance With Bomb",
//	L"Active Camo With Bombe",
//	L"Vehicle Operation"
//};
//
//s_halo_playlist_item g_halo_playlist_assault_property_items[k_halo_playlist_assault_property_item_count]
//{
//	{g_halo_playlist_assault_property_strings[0], _halo_playlist_assault_property_score_to_win_round},
//	{g_halo_playlist_assault_property_strings[1], _halo_playlist_assault_property_team_changing},
//	{g_halo_playlist_assault_property_strings[2], _halo_playlist_assault_property_force_even_teams},
//	{g_halo_playlist_assault_property_strings[3], _halo_playlist_assault_property_bomb_type},
//	{g_halo_playlist_assault_property_strings[4], _halo_playlist_assault_property_enemy_bomb_indicator},
//	{g_halo_playlist_assault_property_strings[5], _halo_playlist_assault_property_sudden_death},
//	{g_halo_playlist_assault_property_strings[6], _halo_playlist_assault_property_bomb_touch_return},
//	{g_halo_playlist_assault_property_strings[7], _halo_playlist_assault_property_bomb_reset_time},
//	{g_halo_playlist_assault_property_strings[8], _halo_playlist_assault_property_bomb_arm_time},
//	{g_halo_playlist_assault_property_strings[9], _halo_playlist_assault_property_sticky_arming},
//	{g_halo_playlist_assault_property_strings[10], _halo_playlist_assault_property_slow_with_bomb},
//	{g_halo_playlist_assault_property_strings[11], _halo_playlist_assault_property_bomb_hit_damage},
//	{g_halo_playlist_assault_property_strings[12], _halo_playlist_assault_property_damage_resistance_with_bomb},
//	{g_halo_playlist_assault_property_strings[13], _halo_playlist_assault_property_active_camo_with_bomb},
//	{g_halo_playlist_assault_property_strings[14], _halo_playlist_assault_property_vehicle_operation}
//};
//
//s_halo_playlist_item_collection g_halo_playlist_assault_property_item_collection = { g_halo_playlist_assault_property_items, k_halo_playlist_assault_property_item_count };

e_halo_playlist_assault_property halo_playlist_item_collection_assault_property_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_assault_property_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x3562A0);
	return (e_halo_playlist_assault_property)halo_playlist_item_collection_get_value(g_halo_playlist_assault_property_item_collection, value);
}
