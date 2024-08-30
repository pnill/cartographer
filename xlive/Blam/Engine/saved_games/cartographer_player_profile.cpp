#include "stdafx.h"
#include "cartographer_player_profile.h"
#include "saved_game_files_async_windows.h"
#include "game/players.h"
#include "input/input_abstraction.h"

// To add a new player profile version, start by creating a new structure for the profile version (e.g., s_saved_game_cartographer_player_profile_v3).
// then specialize the template function cartographer_player_profile_new for the new type to initialize its default values.
// add a upgrade function that converts profiles from the previous version to the new version (e.g., cartographer_player_profile_upgrade_to_v3).
// Update the 'cartographer_player_profile_upgrade_check' function to include a new case for the version upgrade, calling the appropriate upgrade function.
// after doing that go into the header and change the typedef and global value that controls the version used in the codebase.
//
// typedef NEW_STRUCTURE s_saved_game_cartographer_player_profile;
// constexpr e_saved_game_cartographer_player_profile_version g_cartographer_profile_version = NEW_ENUM_VALUE;


/* globals */

s_saved_game_cartographer_player_profile g_default_cartographer_profile;
s_cartographer_profile_run_time g_cartographer_profiles[k_number_of_controllers]{};

constexpr const wchar_t* cartographer_bin = L"cartographer_profile";
constexpr uint32 cartographer_signature = 'cart';

/* private code */


// when making a new version of the cartographer profile create a new template specialization for that type
template<typename T>
void cartographer_player_profile_new(T* settings);

template<>
void cartographer_player_profile_new<s_saved_game_cartographer_player_profile_v1>(s_saved_game_cartographer_player_profile_v1* settings)
{
	settings->version = _saved_game_cartographer_player_profile_version_1;
	settings->signature = cartographer_signature;
}

template<>
void cartographer_player_profile_new<s_saved_game_cartographer_player_profile_v2>(s_saved_game_cartographer_player_profile_v2* settings)
{
	settings->version = _saved_game_cartographer_player_profile_version_2;
	settings->signature = cartographer_signature;
	settings->field_of_view = 78;
	settings->vehicle_field_of_view = 78;
	settings->static_first_person = false;
	settings->mouse_sensitivity = 0;
	settings->raw_mouse_sensitivity = 25.f;
	settings->mouse_uniform = false;
	settings->raw_mouse_input = false;
	settings->controller_sensitivity = 0;
	settings->controller_modern = false;
	settings->controller_deadzone_type = 0;
	settings->deadzone_axial_x = 26.f;
	settings->deadzone_axial_y = 26.f;
	settings->deadzone_radial = 1.f;
	settings->crosshair_offset = 0.138f;
	settings->crosshair_scale = 1.f;
}

// when creating a newer version of the cartographer profile create a function that can upgrade from the previous version to the new one
void cartographer_player_profile_upgrade_to_v2(s_saved_game_cartographer_player_profile* settings)
{
	s_saved_game_cartographer_player_profile_v2 t_settings;
	cartographer_player_profile_new(&t_settings);

	settings->version = _saved_game_cartographer_player_profile_version_2;
	settings->field_of_view = t_settings.field_of_view;
	settings->vehicle_field_of_view = t_settings.vehicle_field_of_view;
	settings->static_first_person = t_settings.static_first_person;
	settings->mouse_sensitivity = t_settings.mouse_sensitivity;
	settings->raw_mouse_sensitivity = t_settings.raw_mouse_sensitivity;
	settings->mouse_uniform = t_settings.mouse_uniform;
	settings->raw_mouse_sensitivity = t_settings.raw_mouse_input;
	settings->controller_sensitivity = t_settings.controller_sensitivity;
	settings->controller_modern = t_settings.controller_modern;
	settings->controller_deadzone_type = t_settings.controller_deadzone_type;
	settings->deadzone_axial_x = t_settings.deadzone_axial_x;
	settings->deadzone_axial_y = t_settings.deadzone_axial_y;
	settings->deadzone_radial = t_settings.deadzone_radial;
	settings->crosshair_offset = t_settings.crosshair_offset;
	settings->crosshair_scale = t_settings.crosshair_scale;
}

bool cartographer_player_profile_upgrade_check(s_saved_game_cartographer_player_profile* settings, bool* out_upgrade_performed)
{
	while(settings->version != g_cartographer_profile_version)
	{
		// when creating a newer version of the cartographer profile create a new case to call the new upgrade to function
		switch(settings->version)
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
	}
	return true;
}

bool cartographer_player_profile_verify(s_saved_game_cartographer_player_profile* settings, e_saved_game_cartographer_player_profile_version* out_version)
{
	if (settings->signature != cartographer_signature)
	{
		*out_version = k_saved_game_cartographer_player_profile_version_invalid;
		return false;
	}
	if (settings->version != g_cartographer_profile_version)
	{
		*out_version = settings->version;
		return false;
	}

	*out_version = settings->version;
	return true;
}

/* public code */

s_saved_game_cartographer_player_profile* cartographer_player_profile_get_by_controller_index(e_controller_index controller_index)
{
	return &g_cartographer_profiles[controller_index].profile;
}

s_saved_game_cartographer_player_profile* cartographer_player_profile_get_by_user_index(uint32 user_index)
{
	s_players_globals* player_globals = get_players_globals();

	if (user_index < player_globals->player_user_count)
	{
		datum user_datum = player_globals->player_user_mapping[user_index];
		for(uint32 index = 0; index < k_number_of_controllers; index++)
		{
			if (player_globals->player_controller_mapping[index] == user_datum && g_cartographer_profiles[index].enumerated_file_index != NONE)
				return &g_cartographer_profiles[index].profile;
		}
	}

	return &g_default_cartographer_profile;
}

void cartographer_player_profile_sign_in(e_controller_index controller_index, uint32 enumerated_file_index)
{
	s_saved_game_cartographer_player_profile* current_profile = &g_cartographer_profiles[controller_index].profile;

	g_cartographer_profiles[controller_index].enumerated_file_index = enumerated_file_index;
	g_cartographer_profiles[controller_index].controller_index = controller_index;

	// try to load the profile
	if(!saved_games_async_helper_read_saved_game_bin(cartographer_bin, 
		enumerated_file_index, 
		(int8*)current_profile, 
		sizeof(s_saved_game_cartographer_player_profile)))
	{
		// profile does not exist or is corrupted
		cartographer_player_profile_new(current_profile);
		saved_games_async_helper_write_saved_game_bin(cartographer_bin,
			enumerated_file_index,
			(int8*)current_profile,
			sizeof(s_saved_game_cartographer_player_profile));
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
				saved_games_async_helper_write_saved_game_bin(cartographer_bin,
					enumerated_file_index,
					(int8*)current_profile,
					sizeof(s_saved_game_cartographer_player_profile));
			}
			else
			{
				bool upgraded_check = false;
				// check if profile needs to be upgraded
				if (cartographer_player_profile_upgrade_check(current_profile, &upgraded_check))
				{
					// if profile was upgraded re-save it.
					if(upgraded_check)
					{
						cartographer_player_profile_save(controller_index);
					}
				}
				else
				{
					// upgrade check failed over-write original.
					cartographer_player_profile_new(current_profile);
					saved_games_async_helper_write_saved_game_bin(cartographer_bin,
						enumerated_file_index,
						(int8*)current_profile,
						sizeof(s_saved_game_cartographer_player_profile));
				}
			}
		}
	}

	input_abstraction_set_controller_look_sensitivity(controller_index, current_profile->controller_sensitivity);
	input_abstraction_set_controller_thumb_deadzone(controller_index);
}

void cartographer_player_profile_sign_out(e_controller_index controller_index)
{
	g_cartographer_profiles[controller_index].enumerated_file_index = NONE;
	g_cartographer_profiles[controller_index].controller_index = k_no_controller;
	g_cartographer_profiles[controller_index].user_index = NONE;
	cartographer_player_profile_new(&g_cartographer_profiles[controller_index].profile);
}

void cartographer_player_profile_save(e_controller_index controller_index)
{
	if (g_cartographer_profiles[controller_index].controller_index != NONE)
		saved_games_async_helper_write_saved_game_bin(cartographer_bin,
			g_cartographer_profiles[controller_index].enumerated_file_index,
			(int8*)&g_cartographer_profiles[controller_index].profile,
			sizeof(s_saved_game_cartographer_player_profile));
}

void cartographer_player_profile_initialize()
{
	for (uint32 index = 0; index < k_number_of_controllers; ++index)
	{
		cartographer_player_profile_new(&g_cartographer_profiles[index].profile);
		g_cartographer_profiles[index].enumerated_file_index = NONE;
		g_cartographer_profiles[index].controller_index = k_no_controller;
		g_cartographer_profiles[index].user_index = NONE;
	}
	cartographer_player_profile_new(&g_default_cartographer_profile);
}


