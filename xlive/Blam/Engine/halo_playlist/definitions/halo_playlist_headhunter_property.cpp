#include "stdafx.h"
#include "halo_playlist_headhunter_property.h"

constexpr wchar_t* g_halo_playlist_headhunter_property_strings[k_halo_playlist_headhunter_property_item_count]
{
	L"Score to Win Round",
	L"Team Play",
	L"Team Scoring",
	L"Team Changing",
	L"Force Even Teams",
	L"Uncontested Hill",
	L"Moving Hill",
	L"Team Time Multiplier",
	L"Extra Damage on Hill",
	L"Damage Resistance on Hill",
	L"Active Camo on Hill",
	L"Max heads carried",
	L"Speed with Heads"
};

s_halo_playlist_item g_halo_playlist_headhunter_property_items[k_halo_playlist_headhunter_property_item_count]
{
	{g_halo_playlist_headhunter_property_strings[0], _halo_playlist_headhunter_property_score_to_win},
	{g_halo_playlist_headhunter_property_strings[1], _halo_playlist_headhunter_property_team_play},
	{g_halo_playlist_headhunter_property_strings[2], _halo_playlist_headhunter_property_team_scoring},
	{g_halo_playlist_headhunter_property_strings[3], _halo_playlist_headhunter_property_team_changing},
	{g_halo_playlist_headhunter_property_strings[4], _halo_playlist_headhunter_property_force_even_teams},
	{g_halo_playlist_headhunter_property_strings[5], _halo_playlist_headhunter_property_uncontested_hill},
	{g_halo_playlist_headhunter_property_strings[6], _halo_playlist_headhunter_property_moving_hill},
	{g_halo_playlist_headhunter_property_strings[7], _halo_playlist_headhunter_property_team_time_multiplier},
	{g_halo_playlist_headhunter_property_strings[8], _halo_playlist_headhunter_property_extra_damage_on_hill},
	{g_halo_playlist_headhunter_property_strings[9], _halo_playlist_headhunter_property_damage_resistance_on_hill},
	{g_halo_playlist_headhunter_property_strings[10], _halo_playlist_headhunter_property_active_camo_on_hill},
	{g_halo_playlist_headhunter_property_strings[11], _halo_playlist_headhunter_property_max_heads_carried},
	{g_halo_playlist_headhunter_property_strings[12], _halo_playlist_headhunter_property_speed_with_heads}
};

s_halo_playlist_item_collection g_halo_playlist_headhunter_property_item_collection{ g_halo_playlist_headhunter_property_items, k_halo_playlist_headhunter_property_item_count };

e_halo_playlist_headhunter_property halo_playlist_item_collection_headhunter_property_get_value(wchar_t* value)
{
	return (e_halo_playlist_headhunter_property)halo_playlist_item_collection_get_value(&g_halo_playlist_headhunter_property_item_collection, value);
}
