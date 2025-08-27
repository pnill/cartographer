#include "stdafx.h"
#include "halo_playlist_equipment_property.h"

//constexpr wchar_t* g_halo_playlist_equipment_property_strings[k_halo_playlist_equipment_property_item_count]
//{
//	L"Starting Weapon",
//	L"Secondary Weapon",
//	L"Starting Grenades",
//	L"Weapons on Map",
//	L"Weapon Respawn Time",
//	L"Grenades on Map",
//	L"Overshields",
//	L"Active Camo on Map"
//};
//
//s_halo_playlist_item g_halo_playlist_equipment_property_items[k_halo_playlist_equipment_property_item_count]
//{
//	{g_halo_playlist_equipment_property_strings[0], _halo_playlist_equipment_property_starting_weapon},
//	{g_halo_playlist_equipment_property_strings[1], _halo_playlist_equipment_property_secondary_weapon},
//	{g_halo_playlist_equipment_property_strings[2], _halo_playlist_equipment_property_starting_grenades},
//	{g_halo_playlist_equipment_property_strings[3], _halo_playlist_equipment_property_weapons_on_map},
//	{g_halo_playlist_equipment_property_strings[4], _halo_playlist_equipment_property_weapon_respawn_time},
//	{g_halo_playlist_equipment_property_strings[5], _halo_playlist_equipment_property_grenades_on_map},
//	{g_halo_playlist_equipment_property_strings[6], _halo_playlist_equipment_property_over_shields_on_map},
//	{g_halo_playlist_equipment_property_strings[7], _halo_playlist_equipment_property_active_camo_on_map}
//};
//
//s_halo_playlist_item_collection g_halo_playlist_equipment_property_item_collection = { g_halo_playlist_equipment_property_items, k_halo_playlist_equipment_property_item_count };

e_halo_playlist_equipment_property halo_playlist_item_collection_equipment_property_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_equipment_property_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355B30);
	return (e_halo_playlist_equipment_property)halo_playlist_item_collection_get_value(g_halo_playlist_equipment_property_item_collection, value);
}
