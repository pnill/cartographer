#include "stdafx.h"
#include "halo_playlist_game_type_property.h"

constexpr wchar_t* g_halo_playlist_game_type_property_strings[k_halo_playlist_game_type_item_count]
{
	L"Slayer",
	L"King of the Hill",
	L"King",
	L"KoTH",
	L"Oddball",
	L"Juggernaut",
	L"Capture the Flag",
	L"CTF",
	L"Assault",
	L"Territories",
	L"Headhunter"
};

s_halo_playlist_item g_halo_playlist_game_type_property_items[k_halo_playlist_game_type_item_count]
{
	{g_halo_playlist_game_type_property_strings[0], _game_variant_description_slayer},
	{g_halo_playlist_game_type_property_strings[1], _game_variant_description_king },
	{g_halo_playlist_game_type_property_strings[2], _game_variant_description_king},
	{g_halo_playlist_game_type_property_strings[3], _game_variant_description_king},
	{g_halo_playlist_game_type_property_strings[4], _game_variant_description_oddball},
	{g_halo_playlist_game_type_property_strings[5], _game_variant_description_juggernaut},
	{g_halo_playlist_game_type_property_strings[6], _game_variant_description_ctf},
	{g_halo_playlist_game_type_property_strings[7], _game_variant_description_ctf},
	{g_halo_playlist_game_type_property_strings[8], _game_variant_description_invasion},
	{g_halo_playlist_game_type_property_strings[9], _game_variant_description_territories},
	{g_halo_playlist_game_type_property_strings[10], _game_variant_description_headhunter}
};

s_halo_playlist_item_collection g_halo_playlist_game_type_property_item_collection{ g_halo_playlist_game_type_property_items, k_halo_playlist_game_type_item_count };

wchar_t* halo_playlist_item_collection_game_type_get_name(e_game_variant_description_index value)
{
	return halo_playlist_item_collection_get_name(&g_halo_playlist_game_type_property_item_collection, value);
}

e_game_variant_description_index halo_playlist_item_collection_game_type_get_value(wchar_t* value)
{
	return (e_game_variant_description_index)halo_playlist_item_collection_get_value(&g_halo_playlist_game_type_property_item_collection, value);
}
