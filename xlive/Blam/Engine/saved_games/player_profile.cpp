#include "stdafx.h"
#include "player_profile.h"

#include "saved_game_files.h"
#include "saved_game_files_async_windows.h"

void __cdecl saved_game_player_profile_set_default_variant(void* saved_game_variant)
{
	INVOKE(0x98FD, 0, saved_game_player_profile_set_default_variant, saved_game_variant);
}

void saved_game_player_profile_set_input_preferences(s_gamepad_input_preferences* input_preferences,
	s_saved_game_profile_input_preferences* profile_input_preferences)
{
	INVOKE(0x61B62, 0, saved_game_player_profile_set_input_preferences, input_preferences, profile_input_preferences);
}

void saved_game_player_profile_default_new(s_saved_game_player_profile* profile, int32 default_profile_type)
{
	s_gamepad_input_preferences input_preferences;

	csmemset(profile, 0, sizeof(s_saved_game_player_profile));
	profile->valid_maybe = true;
	wcsncpy(profile->name, L"Guest", 16);
	profile->profile_traits.profile.primary_color = _player_color_colbat;
	profile->profile_traits.profile.secondary_color = _player_color_white;
	profile->profile_traits.profile.tertiary_color = _player_color_white;
	profile->profile_traits.profile.quaternary_color = _player_color_white;
	profile->profile_traits.profile.emblem_info.foreground_emblem = _emblem_foreground_seventh_column;
	profile->profile_traits.profile.emblem_info.background_emblem = _emblem_background_solid;
	profile->unk |= 1;
	profile->input_preferences.controller_sensitivity = 3;
	profile->input_preferences.mouse_sensitivity = 3;
	profile->input_preferences.flags.clear();

	if(default_profile_type)
	{
		if(default_profile_type == 1)
		{
			profile->input_preferences.controller_button_layout = 0;
			profile->input_preferences.controller_thumbstick_layout = 0;
			profile->input_preferences.flags.set(_saved_game_profile_input_preference_flag_controller_look_inversion, true);
			profile->input_preferences.flags.set(_saved_game_profile_input_preference_flag_mouse_look_inversion, true);
		}
	}
	else
	{
		profile->input_preferences.controller_button_layout = 0;
		profile->input_preferences.controller_thumbstick_layout = 0;
	}
	saved_game_player_profile_set_default_variant(&profile->variant);
	input_abstraction_preferences_new(&input_preferences, 0, false, false);
	saved_game_player_profile_set_input_preferences(&input_preferences, &profile->input_preferences);
}

bool saved_game_player_profile_read_file(uint32 enumerated_file_index, s_saved_game_player_profile* profile)
{
	ASSERT(profile);

	int8 buffer[sizeof(s_saved_game_player_profile)];

	bool result = saved_games_async_helper_read_file(enumerated_file_index, buffer,sizeof(s_saved_game_player_profile));

	if (result)
		csmemcpy(profile, buffer, sizeof(s_saved_game_player_profile));

	// todo: cleanup and define the structure here
	//*(int16*)profile->data[0x1E] = 0;
	//if (profile->gap2[0x29] >= 3u)
	//	profile->gap2[0x29] = 0;

	if(!saved_game_player_profile_read_post_verify_profile_traits(&profile->profile_traits))
		csmemset(&profile->profile_traits, 0, sizeof(s_player_profile_traits));
	
	return result;
}

bool __cdecl saved_game_player_profile_read_post_verify_profile_traits(s_player_profile_traits* profile_traits)
{
	return INVOKE(0x54f82, 0, saved_game_player_profile_read_post_verify_profile_traits, profile_traits);
}

bool saved_game_player_profile_load(uint32 enumerated_file_index, s_saved_game_player_profile* profile)
{
	ASSERT(profile);

	if(enumerated_file_index == NONE)
	{
		saved_game_player_profile_default_new(profile, 0);
		return true;
	}
	else
	{
		return saved_game_player_profile_read_file(enumerated_file_index, profile);
	}
}
