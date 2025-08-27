#include "stdafx.h"
#include "halo_playlist_variant_property.h"

//constexpr wchar_t* g_halo_playlist_variant_property_strings[k_halo_playlist_variant_property_item_count]
//{
//	L"Name",
//	L"Base Variant",
//	L"Game Type"
//};
//
//s_halo_playlist_item g_halo_playlist_variant_property_items[k_halo_playlist_variant_property_item_count]
//{
//	{g_halo_playlist_variant_property_strings[0], _halo_playlist_variant_property_name},
//	{g_halo_playlist_variant_property_strings[1], _halo_playlist_variant_property_base_variant},
//	{g_halo_playlist_variant_property_strings[2], _halo_playlist_variant_property_game_type}
//};
//
//s_halo_playlist_item_collection g_halo_playlist_variant_property_item_collection{ g_halo_playlist_variant_property_items, k_halo_playlist_variant_property_item_count };

e_halo_playlist_variant_property_type halo_playlist_item_collection_get_variant_property_type(wchar_t* value)
{
	s_halo_playlist_item_collection* g_halo_playlist_variant_property_item_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355338);

	return (e_halo_playlist_variant_property_type)halo_playlist_item_collection_get_value(g_halo_playlist_variant_property_item_collection, value);
}
