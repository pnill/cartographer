#include "stdafx.h"
#include "cartographer_player_profile.h"

#include "saved_games/saved_game_files.h"
#include "saved_games/saved_game_files_async_windows.h"

#include "game/players.h"
#include "input/input_abstraction.h"

/*
* To add a new player profile version, start by creating a new structure for the profile version (e.g., s_saved_game_cartographer_player_profile_v3).
* then specialize the template function cartographer_player_profile_new for the new type to initialize its default values.
* add a upgrade function that converts profiles from the previous version to the new version (e.g., cartographer_player_profile_upgrade_to_v3).
* Update the 'cartographer_player_profile_upgrade_check' function to include a new case for the version upgrade, calling the appropriate upgrade function.
* after doing that go into the header and change the typedef and global value that controls the version used in the codebase.
* typedef NEW_STRUCTURE s_saved_game_cartographer_player_profile;
* constexpr e_saved_game_cartographer_player_profile_version k_cartographer_profile_version = NEW_ENUM_VALUE;
*/

/* constants */

enum
{
	k_cartographer_profile_signature = 'cart'
};

static const wchar_t k_cartographer_bin_name[] = L"cartographer_profile";

/* globals */

s_saved_game_cartographer_player_profile g_default_cartographer_profile = {};
s_cartographer_profile_run_time g_cartographer_profiles[k_number_of_controllers] = {};

/* prototypes */

static void cartographer_player_profile_header_new(s_saved_game_cartographer_player_profile_main_header* header, e_saved_game_cartographer_player_profile_version version);

static void cartographer_player_profile_new(s_saved_game_cartographer_player_profile_v1* settings);

static void cartographer_player_profile_new(s_saved_game_cartographer_player_profile_v2* settings);

static void cartographer_player_profile_new(s_saved_game_cartographer_player_profile_v3* settings);

// when creating a newer version of the cartographer profile create a function that can upgrade from the previous version to the new one
static void cartographer_player_profile_upgrade_to_v2(s_saved_game_cartographer_player_profile* settings);

static void cartographer_player_profile_upgrade_to_v3(s_saved_game_cartographer_player_profile* settings);

static bool cartographer_player_profile_upgrade_check(s_saved_game_cartographer_player_profile* settings, bool* out_upgrade_performed);

static bool cartographer_player_profile_verify(s_saved_game_cartographer_player_profile* settings, e_saved_game_cartographer_player_profile_version* out_version);

/* public code */

void cartographer_player_profile_initialize(void)
{
	for (uint32 index = 0; index < k_number_of_controllers; ++index)
	{
		s_cartographer_profile_run_time* profile = &g_cartographer_profiles[index];
		cartographer_player_profile_new(&profile->profile);
		profile->enumerated_file_index = NONE;
		profile->controller_index = k_no_controller;
		profile->user_index = NONE;
	}
	cartographer_player_profile_new(&g_default_cartographer_profile);
}

bool cartographer_player_profile_is_signed_in(e_controller_index controller_index)
{
	if (controller_index == k_no_controller)
		return false;

	s_cartographer_profile_run_time* profile = &g_cartographer_profiles[controller_index];

	if (ENUMERATED_INDEX_IS_DEFAULT_SAVE(profile->enumerated_file_index) && profile->enumerated_file_index != NONE)
		return true;

	return profile->controller_index != k_no_controller;
}

s_saved_game_cartographer_player_profile* cartographer_player_profile_get_by_controller_index(e_controller_index controller_index)
{
	s_cartographer_profile_run_time* profile = &g_cartographer_profiles[controller_index];
	if (ENUMERATED_INDEX_IS_DEFAULT_SAVE(profile->enumerated_file_index))
		return &g_default_cartographer_profile;

	return &profile->profile;
}

s_saved_game_cartographer_player_profile* cartographer_player_profile_get_by_user_index(int32 user_index)
{
	const s_players_globals* player_globals = get_players_globals();

	if ((int16)user_index < player_globals->local_player_count)
	{
		datum user_datum = player_globals->player_user_mapping[user_index];
		for (uint32 index = 0; index < k_number_of_controllers; ++index)
		{
			s_cartographer_profile_run_time* profile = &g_cartographer_profiles[index];
			if (player_globals->player_controller_mapping[index] == user_datum && profile->enumerated_file_index != NONE && !ENUMERATED_INDEX_IS_DEFAULT_SAVE(profile->enumerated_file_index))
			{
				return &profile->profile;
			}
		}
	}

	return &g_default_cartographer_profile;
}

void cartographer_player_profile_sign_in(e_controller_index controller_index, int32 enumerated_file_index)
{
	// The game will re-sign in the profiles when switching between maps no need to
	// re-read the profile binary unless the player has actually signed out
	s_cartographer_profile_run_time* profile = &g_cartographer_profiles[controller_index];
	if (enumerated_file_index != profile->enumerated_file_index)
	{
		s_saved_game_cartographer_player_profile* current_profile_data = &profile->profile;

		profile->enumerated_file_index = enumerated_file_index;
		profile->controller_index = controller_index;

		if (!ENUMERATED_INDEX_IS_DEFAULT_SAVE(enumerated_file_index))
		{
			// try to load the profile
			if (!saved_games_async_helper_read_saved_game_bin(k_cartographer_bin_name,
				enumerated_file_index,
				(int8*)current_profile_data,
				sizeof(s_saved_game_cartographer_player_profile)))
			{
				// profile does not exist or is corrupted
				cartographer_player_profile_new(current_profile_data);
				cartographer_player_profile_save(controller_index);
			}
			else
			{
				// check the profile is valid
				e_saved_game_cartographer_player_profile_version version;
				if (!cartographer_player_profile_verify(current_profile_data, &version))
				{
					// if the loaded profile is deemed invalid over-write it.
					if (version == k_saved_game_cartographer_player_profile_version_invalid)
					{
						cartographer_player_profile_new(current_profile_data);
						cartographer_player_profile_save(controller_index);
					}
					else
					{
						bool upgraded_check = false;
						// check if profile needs to be upgraded
						if (cartographer_player_profile_upgrade_check(current_profile_data, &upgraded_check))
						{
							// if profile was upgraded re-save it.
							if (upgraded_check)
							{
								cartographer_player_profile_save(controller_index);
							}
						}
						else
						{
							// upgrade check failed over-write original.
							cartographer_player_profile_new(current_profile_data);
							cartographer_player_profile_save(controller_index);
						}
					}
				}
			}
		}
	}

	// always redo the input abstraction methods for custom values
	// to write the custom settings into the input_preferences
	input_abstraction_set_controller_look_sensitivity(controller_index, profile->profile.controller_sensitivity);
	input_abstraction_set_controller_right_thumb_deadzone(controller_index);
}

void cartographer_player_profile_sign_out(e_controller_index controller_index)
{
	s_cartographer_profile_run_time* profile = &g_cartographer_profiles[controller_index];
	profile->enumerated_file_index = NONE;
	profile->controller_index = k_no_controller;
	profile->user_index = NONE;
	cartographer_player_profile_new(&profile->profile);
}

void cartographer_player_profile_save(e_controller_index controller_index)
{
	s_cartographer_profile_run_time* profile = &g_cartographer_profiles[controller_index];
	if (!ENUMERATED_INDEX_IS_DEFAULT_SAVE(profile->enumerated_file_index) && profile->controller_index != NONE)
	{
		saved_games_async_helper_write_saved_game_bin(k_cartographer_bin_name,
			profile->enumerated_file_index,
			(int8*)&profile->profile,
			sizeof(s_saved_game_cartographer_player_profile));
	}
}

/* private code */

static void cartographer_player_profile_header_new(s_saved_game_cartographer_player_profile_main_header* header, e_saved_game_cartographer_player_profile_version version)
{
	header->version = version;
	header->signature = k_cartographer_profile_signature;
}

static void cartographer_player_profile_new(s_saved_game_cartographer_player_profile_v1* settings)
{
	cartographer_player_profile_header_new(&settings->header, _saved_game_cartographer_player_profile_version_1);
}

static void cartographer_player_profile_new(s_saved_game_cartographer_player_profile_v2* settings)
{
	cartographer_player_profile_header_new(&settings->header, _saved_game_cartographer_player_profile_version_2);

	settings->field_of_view = 78;
	settings->vehicle_field_of_view = 78;
	settings->static_first_person = false;
	settings->mouse_sensitivity = 0.f;
	settings->raw_mouse_sensitivity = 25.f;
	settings->mouse_uniform = false;
	settings->raw_mouse_input = false;
	settings->controller_sensitivity = 0.f;
	settings->controller_modern = false;
	settings->controller_deadzone_type = _controller_deadzone_type_axial;
	settings->deadzone_axial = k_default_right_thumbstick_deadzone_axial_percentage;
	// set this default to 8 percent
	settings->deadzone_radial = k_default_right_thumbstick_deadzone_radial_percentage;
	settings->crosshair_offset = 0.138f;
	settings->crosshair_scale = 1.f;
}

static void cartographer_player_profile_new(s_saved_game_cartographer_player_profile_v3* settings)
{
	cartographer_player_profile_header_new(&settings->header, _saved_game_cartographer_player_profile_version_3);

	settings->field_of_view = 78;
	settings->vehicle_field_of_view = 78;
	settings->static_first_person = false;
	settings->mouse_sensitivity = 0.f;
	settings->raw_mouse_sensitivity = 25.f;
	settings->mouse_uniform = false;
	settings->raw_mouse_input = false;
	settings->controller_sensitivity = 0.f;
	settings->controller_modern = false;
	settings->controller_deadzone_type = _controller_deadzone_type_axial;
	settings->deadzone_axial = k_default_right_thumbstick_deadzone_axial_percentage;

	// set this default to 8 percent
	settings->deadzone_radial = k_default_right_thumbstick_deadzone_radial_percentage;
	settings->crosshair_offset = 0.138f;
	settings->crosshair_scale = 1.f;

	for (size_t i = 0; i < k_weapon_offset_weapon_count; ++i)
	{
		settings->weapon_offsets[i] = k_weapon_offset_constant_data[i].default_offset;
	}
}

static void cartographer_player_profile_upgrade_to_v2(s_saved_game_cartographer_player_profile* settings)
{
	cartographer_player_profile_new((s_saved_game_cartographer_player_profile_v2*)settings);
}

static void cartographer_player_profile_upgrade_to_v3(s_saved_game_cartographer_player_profile* settings)
{
	// Make a copy of the old version as we're overwritting the buffer below
	s_saved_game_cartographer_player_profile_v2 old_settings = *(s_saved_game_cartographer_player_profile_v2*)settings;

	cartographer_player_profile_new(settings);

	settings->field_of_view = old_settings.field_of_view;
	settings->vehicle_field_of_view = old_settings.vehicle_field_of_view;
	settings->static_first_person = old_settings.static_first_person;
	settings->mouse_sensitivity = old_settings.mouse_sensitivity;
	settings->raw_mouse_sensitivity = old_settings.raw_mouse_sensitivity;
	settings->mouse_uniform = old_settings.mouse_uniform;
	settings->raw_mouse_input = old_settings.raw_mouse_input;
	settings->controller_sensitivity = old_settings.controller_sensitivity;
	settings->custom_layout = old_settings.custom_layout;
	settings->controller_modern = old_settings.controller_modern;
	settings->controller_deadzone_type = old_settings.controller_deadzone_type;
	settings->deadzone_axial = old_settings.deadzone_axial;
	settings->deadzone_radial = old_settings.deadzone_radial;
	settings->crosshair_offset = old_settings.crosshair_offset;
	settings->crosshair_scale = old_settings.crosshair_scale;
}

static bool cartographer_player_profile_upgrade_check(s_saved_game_cartographer_player_profile* settings, bool* out_upgrade_performed)
{
	bool result = true;
	uint32 upgrade_iterations = 0;

	while(settings->header.version != k_cartographer_profile_version)
	{
		// when creating a newer version of the cartographer profile create a new case to call the new upgrade to function
		switch(settings->header.version)
		{
		case _saved_game_cartographer_player_profile_version_1:
			cartographer_player_profile_upgrade_to_v2(settings);
			*out_upgrade_performed = true;
			break;
		case _saved_game_cartographer_player_profile_version_2:
			cartographer_player_profile_upgrade_to_v3(settings);
			*out_upgrade_performed = true;
			break;
		case _saved_game_cartographer_player_profile_version_3:
			break;
		default:
			DISPLAY_ASSERT("A cartographer profile version that was un-expected was reached during upgrading!!!");
			result = false;
			break;
		}

		if(++upgrade_iterations == k_saved_game_cartographer_player_profile_version_count)
		{
			DISPLAY_ASSERT("Maximum iterations of possible cartographer profile upgrades was reached without success, assuming profile is corrupt.");
			result = false;
			break;
		}
	}

	return result;
}

static bool cartographer_player_profile_verify(s_saved_game_cartographer_player_profile* settings, e_saved_game_cartographer_player_profile_version* out_version)
{
	if (settings->header.signature != k_cartographer_profile_signature)
	{
		*out_version = k_saved_game_cartographer_player_profile_version_invalid;
		return false;
	}
	if (settings->header.version != k_cartographer_profile_version)
	{
		*out_version = settings->header.version;
		return false;
	}

	*out_version = settings->header.version;
	return true;
}
