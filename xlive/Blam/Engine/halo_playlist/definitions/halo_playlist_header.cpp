#include "stdafx.h"
#include "halo_playlist_header.h"

//constexpr wchar_t* g_halo_playlist_header_strings[k_halo_playlist_header_item_count]
//{
//	L"Playlist",
//	L"Variant",
//	L"Custom Game",
//	L"Custom Variant",
//	L"Match"
//};
//
//s_halo_playlist_item g_playlist_header_items[k_halo_playlist_header_item_count]
//{
//	{g_halo_playlist_header_strings[0], _halo_playlist_header_playlist},
//	{g_halo_playlist_header_strings[1], _halo_playlist_header_variant},
//	{g_halo_playlist_header_strings[2], _halo_playlist_header_variant},
//	{g_halo_playlist_header_strings[3], _halo_playlist_header_variant},
//	{g_halo_playlist_header_strings[4], _halo_playlist_header_match}
//};
//
//s_halo_playlist_item_collection g_playlist_header_collection = { g_playlist_header_items, k_halo_playlist_header_item_count };

e_halo_playlist_header_type halo_playlist_item_collection_get_header_type(wchar_t* value)
{
	s_halo_playlist_item_collection* g_playlist_header_collection = Memory::GetAddress<s_halo_playlist_item_collection*>(0, 0x355200);

	return (e_halo_playlist_header_type)halo_playlist_item_collection_get_value(g_playlist_header_collection, value);
	//return INVOKE_TYPE(0, 0xEE41, e_halo_playlist_header_type(*)(s_halo_playlist_item_collection*, wchar_t*), g_playlist_header_collection, value);
}
