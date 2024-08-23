#include "stdafx.h"
#include "cartographer_player_profile.h"

#include "saved_game_files.h"
#include "saved_game_files_async_windows.h"

#include "game/players.h"
#include "input/input_abstraction.h"

/*
* To add a new player profile version, start by creating a new structure for the profile version (e.g., s_saved_game_cartographer_player_profile_v3).
* then specialize the template function cartographer_player_profile_new for the new type to initialize its default values.
* add a upgrade function that converts profiles from the previous version to the new version (e.g., cartographer_player_profile_upgrade_to_v3).
* Update the 'cartographer_player_profile_upgrade_check' function to include a new case for the version upgrade, calling the appropriate upgrade function.
* after doing that go into the header and change the typedef and global value that controls the version used in the codebase.
* typedef NEW_STRUCTURE s_saved_game_cartographer_player_profile;
* constexpr e_saved_game_cartographer_player_profile_version g_cartographer_profile_version = NEW_ENUM_VALUE;
*/

/* constants */

#define k_cartographer_profile_signature 'cart'
#define k_cartographer_bin_name L"cartographer_profile"

/* globals */

s_saved_game_cartographer_player_profile g_default_cartographer_profile = {};
s_cartographer_profile_run_time g_cartographer_profiles[k_number_of_controllers] = {};

/* prototypes */

void cartographer_player_profile_new(s_saved_game_cartographer_player_profile_v1* settings);

void cartographer_player_profile_new(s_saved_game_cartographer_player_profile_v2* settings);

// when creating a newer version of the cartographer profile create a function that can upgrade from the previous version to the new one
void cartographer_player_profile_upgrade_to_v2(s_saved_game_cartographer_player_profile* settings);

bool cartographer_player_profile_upgrade_check(s_saved_game_cartographer_player_profile* settings, bool* out_upgrade_performed);

bool cartographer_player_profile_verify(s_saved_game_cartographer_player_profile* settings, e_saved_game_cartographer_player_profile_version* out_version);

/* public code */

void cartographer_player_profile_initialize(void)
{
	for (uint32 index = 0; index < k_number_of_controllers; ++index)
	{
		cartographer_player_profile_new(&g_cartographer_profiles[index].profile);
		g_cartographer_profiles[index].enumerated_file_index = NONE;
		g_cartographer_profiles[index].controller_index = k_no_controller;
		g_cartographer_profiles[index].user_index = NONE;
	}
	cartographer_player_profile_new(&g_default_cartographer_profile);
	return;
}

bool cartographer_player_profile_is_signed_in(e_controller_index controller_index)
{

	if (controller_index == k_no_controller)
		return false;

	if (ENUMERATED_INDEX_IS_DEFAULT_SAVE(g_cartographer_profiles[controller_index].enumerated_file_index) &&
		g_cartographer_profiles[controller_index].enumerated_file_index != NONE)
		return true;

	return g_cartographer_profiles[controller_index].controller_index != k_no_controller;
}

s_saved_game_cartographer_player_profile* cartographer_player_profile_get_by_controller_index(e_controller_index controller_index)
{
	if (ENUMERATED_INDEX_IS_DEFAULT_SAVE(g_cartographer_profiles[controller_index].enumerated_file_index))
		return &g_default_cartographer_profile;

	return &g_cartographer_profiles[controller_index].profile;
}

s_saved_game_cartographer_player_profile* cartographer_player_profile_get_by_user_index(uint32 user_index)
{

	s_players_globals* player_globals = get_players_globals();

	if (user_index < player_globals->local_player_count)
	{
		datum user_datum = player_globals->player_user_mapping[user_index];
		for (uint32 index = 0; index < k_number_of_controllers; index++)
		{
			if (player_globals->player_controller_mapping[index] == user_datum &&
				g_cartographer_profiles[index].enumerated_file_index != NONE &&
				!ENUMERATED_INDEX_IS_DEFAULT_SAVE(g_cartographer_profiles[index].enumerated_file_index))
			{
					return &g_cartographer_profiles[index].profile;
			}
		}
	}

	return &g_default_cartographer_profile;
}

void cartographer_player_profile_sign_in(e_controller_index controller_index, uint32 enumerated_file_index)
{
	// The game will re-sign in the profiles when switching between maps no need to
	// re-read the profile binary unless the player has actually signed out
	if (enumerated_file_index == g_cartographer_profiles[controller_index].enumerated_file_index)
		return;

	s_saved_game_cartographer_player_profile* current_profile = &g_cartographer_profiles[controller_index].profile;

	g_cartographer_profiles[controller_index].enumerated_file_index = enumerated_file_index;
	g_cartographer_profiles[controller_index].controller_index = controller_index;

	if (!ENUMERATED_INDEX_IS_DEFAULT_SAVE(enumerated_file_index))
	{
		// try to load the profile
		if (!saved_games_async_helper_read_saved_game_bin(k_cartographer_bin_name,
			enumerated_file_index,
			(int8*)current_profile,
			sizeof(s_saved_game_cartographer_player_profile)))
		{
			// profile does not exist or is corrupted
			cartographer_player_profile_new(current_profile);
			cartographer_player_profile_save(controller_index);
		}
		else
		{
			// check the profile is valid
			e_saved_game_cartographer_player_profile_version version;
			if (!cartographer_player_profile_verify(current_profile, &version))
			{
				// if the loaded profile is deemed invalid over-write it.
				if (version == k_saved_game_cartographer_player_profile_version_invalid)
				{
					cartographer_player_profile_new(current_profile);
					cartographer_player_profile_save(controller_index);
				}
				else
				{
					bool upgraded_check = false;
					// check if profile needs to be upgraded
					if (cartographer_player_profile_upgrade_check(current_profile, &upgraded_check))
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
						cartographer_player_profile_new(current_profile);
						cartographer_player_profile_save(controller_index);
					}
				}
			}
		}
	}

	input_abstraction_set_controller_look_sensitivity(controller_index, current_profile->controller_sensitivity);
	input_abstraction_set_controller_thumb_deadzone(controller_index);
	return;
}

void cartographer_player_profile_sign_out(e_controller_index controller_index)
{
	g_cartographer_profiles[controller_index].enumerated_file_index = NONE;
	g_cartographer_profiles[controller_index].controller_index = k_no_controller;
	g_cartographer_profiles[controller_index].user_index = NONE;
	cartographer_player_profile_new(&g_cartographer_profiles[controller_index].profile);
	return;
}

void cartographer_player_profile_save(e_controller_index controller_index)
{
	if (!ENUMERATED_INDEX_IS_DEFAULT_SAVE(g_cartographer_profiles[controller_index].enumerated_file_index) &&
		g_cartographer_profiles[controller_index].controller_index != NONE)
	{
		saved_games_async_helper_write_saved_game_bin(k_cartographer_bin_name,
			g_cartographer_profiles[controller_index].enumerated_file_index,
			(int8*)&g_cartographer_profiles[controller_index].profile,
			sizeof(s_saved_game_cartographer_player_profile));
	}
	return;
}

/* private code */

void cartographer_player_profile_new(s_saved_game_cartographer_player_profile_v1* settings)
{
	settings->header.version = _saved_game_cartographer_player_profile_version_1;
	settings->header.signature = k_cartographer_profile_signature;
	return;
}

void cartographer_player_profile_new(s_saved_game_cartographer_player_profile_v2* settings)
{
	settings->header.version = _saved_game_cartographer_player_profile_version_2;
	settings->header.signature = k_cartographer_profile_signature;

	settings->field_of_view = 78;
	settings->vehicle_field_of_view = 78;
	settings->static_first_person = false;
	settings->mouse_sensitivity = 0.f;
	settings->raw_mouse_sensitivity = 25.f;
	settings->mouse_uniform = false;
	settings->raw_mouse_input = false;
	settings->controller_sensitivity = 0.f;
	settings->controller_modern = false;
	settings->controller_deadzone_type = 0.f;
	settings->deadzone_axial_x = 26.f;
	settings->deadzone_axial_y = 26.f;
	settings->deadzone_radial = 1.f;
	settings->crosshair_offset = 0.138f;
	settings->crosshair_scale = 1.f;
	return;
}

void cartographer_player_profile_upgrade_to_v2(s_saved_game_cartographer_player_profile* settings)
{
	cartographer_player_profile_new(settings);
	return;
}

bool cartographer_player_profile_upgrade_check(s_saved_game_cartographer_player_profile* settings, bool* out_upgrade_performed)
{
	uint32 upgrade_iterations = 0;

	while(settings->header.version != g_cartographer_profile_version)
	{
		// when creating a newer version of the cartographer profile create a new case to call the new upgrade to function
		switch(settings->header.version)
		{
		case _saved_game_cartographer_player_profile_version_1:
			cartographer_player_profile_upgrade_to_v2(settings);
			*out_upgrade_performed = true;
			break;
		case _saved_game_cartographer_player_profile_version_2:
			break;
		default:
			DISPLAY_ASSERT("A cartographer profile version that was un-expected was reached during upgrading!!!");
			return false;
		}

		if(++upgrade_iterations == k_saved_game_cartographer_player_profile_version_count)
		{
			DISPLAY_ASSERT("Maximum iterations of possible cartographer profile upgrades was reached without success, assuming profile is corrupt.");
			return false;
		}
	}
	return true;
}

bool cartographer_player_profile_verify(s_saved_game_cartographer_player_profile* settings, e_saved_game_cartographer_player_profile_version* out_version)
{
	if (settings->header.signature != k_cartographer_profile_signature)
	{
		*out_version = k_saved_game_cartographer_player_profile_version_invalid;
		return false;
	}
	if (settings->header.version != g_cartographer_profile_version)
	{
		*out_version = settings->header.version;
		return false;
	}

	*out_version = settings->header.version;
	return true;
}
