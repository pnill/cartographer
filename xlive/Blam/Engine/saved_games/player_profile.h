#pragma once

#include "cseries/cseries.h"
#include "cseries/cseries_strings.h"
#include "game/players.h"

#define k_default_profiles_count 1


struct s_saved_game_controller_settings
{
	PAD(3660);
};
ASSERT_STRUCT_SIZE(s_saved_game_controller_settings, 3660);

struct s_saved_game_file_player_profile
{
	uint32 field_0;
	uint32 field_4;
	c_static_wchar_string32 player_name;
	c_static_string<16> gamertag;
	PAD(4);
	uint32 m_last_level_map_id;
	PAD(136);
	uint8 search_data[112];
	c_static_wchar_string128 search_description;
	PAD(148);
	s_saved_game_controller_settings controller_settings;
	PAD(16);
	s_player_profile_traits profile;
	PAD(176);
};
ASSERT_STRUCT_SIZE(s_saved_game_file_player_profile, 4616);
