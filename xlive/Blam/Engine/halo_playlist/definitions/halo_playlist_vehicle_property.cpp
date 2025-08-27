#include "stdafx.h"
#include "halo_playlist_vehicle_property.h"

//constexpr wchar_t* g_halo_playlist_vehicle_property_strings[k_halo_playlist_vehicle_property_item_count]
//{
//	L"Vehicle Respawn Time",
//	L"Primary Light Vehicle",
//	L"Secondary Light Vehicle",
//	L"Primary Heavy Vehicle",
//	L"Banshee",
//	L"Primary Turret",
//	L"Secondary Turret"
//};
//
//s_halo_playlist_item g_halo_playlist_vehicle_property_items[k_halo_playlist_vehicle_property_item_count]
//{
//	{g_halo_playlist_vehicle_property_strings[0], _halo_playlist_vehicle_property_respawn_time},
//	{g_halo_playlist_vehicle_property_strings[1], _halo_playlist_vehicle_property_primary_light_vehicle},
//	{g_halo_playlist_vehicle_property_strings[2], _halo_playlist_vehicle_property_secondary_light_vehicle},
//	{g_halo_playlist_vehicle_property_strings[3], _halo_playlist_vehicle_property_primary_heavy_vehicle},
//	{g_halo_playlist_vehicle_property_strings[4], _halo_playlist_vehicle_property_banshee},
//	{g_halo_playlist_vehicle_property_strings[5], _halo_playlist_vehicle_property_primary_turret},
//	{g_halo_playlist_vehicle_property_strings[6], _halo_playlist_vehicle_property_secondary_turret},
//};
//
//s_halo_playlist_item_collection g_halo_playlist_vehicle_property_item_collection = { g_halo_playlist_vehicle_property_items, k_halo_playlist_vehicle_property_item_count };

e_halo_playlist_vehicle_property halo_playlist_item_collection_vehicle_property_get_value(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_vehicle_property_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355890);
	return (e_halo_playlist_vehicle_property)halo_playlist_item_collection_get_value(g_halo_playlist_vehicle_property_item_collection, value);
}
