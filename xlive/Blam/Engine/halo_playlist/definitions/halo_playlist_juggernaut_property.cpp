#include "stdafx.h"
#include "halo_playlist_juggernaut_property.h"

//constexpr wchar_t* g_halo_playlist_juggernaut_property_strings[k_halo_playlist_juggernaut_property_item_count]
//{
//	L"Score to Win Round",
//	L"Betrayal Point Loss",
//	L"Juggernaut Extra Damage",
//	L"Juggernaut Infinite Ammo",
//	L"Juggernaut Overshield",
//	L"Juggernaut Active Camo",
//	L"Juggernaut Motion Sensor",
//	L"Juggernaut Movement",
//	L"Juggernaut Damage Resistance"
//};
//
//s_halo_playlist_item g_halo_playlist_juggernaut_property_items[k_halo_playlist_juggernaut_property_item_count]
//{
//	{g_halo_playlist_juggernaut_property_strings[0], _halo_playlist_juggernaut_property_score_to_win_round},
//	{g_halo_playlist_juggernaut_property_strings[1], _halo_playlist_juggernaut_property_betrayal_point_loss},
//	{g_halo_playlist_juggernaut_property_strings[2], _halo_playlist_juggernaut_property_juggernaut_extra_damage},
//	{g_halo_playlist_juggernaut_property_strings[3], _halo_playlist_juggernaut_property_juggernaut_infinite_ammo},
//	{g_halo_playlist_juggernaut_property_strings[4], _halo_playlist_juggernaut_property_juggernaut_overshield},
//	{g_halo_playlist_juggernaut_property_strings[5], _halo_playlist_juggernaut_property_juggernaut_active_came},
//	{g_halo_playlist_juggernaut_property_strings[6], _halo_playlist_juggernaut_property_juggernaut_motion_sensor},
//	{g_halo_playlist_juggernaut_property_strings[7], _halo_playlist_juggernaut_property_juggernaut_movement},
//	{g_halo_playlist_juggernaut_property_strings[8], _halo_playlist_juggernaut_property_juggernaut_damage_resistance}
//};
//
//s_halo_playlist_item_collection g_halo_playlist_juggernaut_property_item_collection = { g_halo_playlist_juggernaut_property_items, k_halo_playlist_juggernaut_property_item_count };

e_halo_playlist_juggernaut_property halo_playlist_item_collection_juggernaut_property_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_juggernaut_property_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355F30);
	return (e_halo_playlist_juggernaut_property)halo_playlist_item_collection_get_value(g_halo_playlist_juggernaut_property_item_collection, value);
}
