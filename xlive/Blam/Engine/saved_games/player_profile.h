#pragma once

#include "cseries/cseries.h"
#include "game/players.h"
#include "input/input_abstraction.h"

#define k_default_profiles_count 1


struct s_saved_game_controller_settings_item
{
	uint8 gap_0[0x40];
};
ASSERT_STRUCT_SIZE(s_saved_game_controller_settings_item, 64);

struct s_saved_game_controller_settings
{
	uint32 field_0;
	uint32 flags;
	uint32 field_8;
	s_saved_game_controller_settings_item items[NUMBER_OF_EXTENDED_CONTROL_BUTTONS];
};
ASSERT_STRUCT_SIZE(s_saved_game_controller_settings, 3660);

struct s_saved_game_file_player_profile
{
	uint32 field_0;
	uint32 field_4;
	wchar_t player_name[32];
	char gamertag[XUSER_NAME_SIZE];
	uint8 gap_58[4];
	uint32 m_last_level_map_id;
	uint8 gap_60[136];
	uint8 search_data[112];
	wchar_t search_description[128];
	uint8 gap_258[148];
	s_saved_game_controller_settings controller_settings;
	uint8 gap_1138[16];
	s_player_profile_traits profile;
	uint8 gap_1158[176];
};
ASSERT_STRUCT_SIZE(s_saved_game_file_player_profile, 4616);
