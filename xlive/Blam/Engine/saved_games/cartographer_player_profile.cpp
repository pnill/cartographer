#include "stdafx.h"
#include "cartographer_player_profile.h"
#include "saved_game_files_async_windows.h"
#include "input/input_abstraction.h"

s_saved_game_cartographer_player_profile_v1 g_cartographer_profile_settings[k_number_of_controllers]{};
uint32 g_cartographer_profile_file_index_map[k_number_of_controllers]{};

constexpr const wchar_t* cartographer_bin = L"cartographer_profile";

void saved_games_cartographer_player_profile_v1_new(s_saved_game_cartographer_player_profile_v1* settings)
{
	settings->version = _saved_game_cartographer_player_profile_version_1;
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
	settings->crosshair_offset = NAN;
	settings->crosshair_scale = 1.f;
}

s_saved_game_cartographer_player_profile_v1* cartographer_player_profile_get(e_controller_index controller_index)
{
	return &g_cartographer_profile_settings[controller_index];
}

void cartographer_player_profile_load(e_controller_index controller_index, uint32 enumerated_file_index)
{
	g_cartographer_profile_file_index_map[controller_index] = enumerated_file_index;

	s_saved_game_cartographer_player_profile_v1* current_profile = &g_cartographer_profile_settings[controller_index];

	if(!saved_games_async_helper_read_saved_game_bin(cartographer_bin, 
		enumerated_file_index, 
		(int8*)current_profile, 
		sizeof(s_saved_game_cartographer_player_profile_v1)))
	{
		saved_games_cartographer_player_profile_v1_new(current_profile);
		saved_games_async_helper_write_saved_game_bin(cartographer_bin, 
			enumerated_file_index, 
			(int8*)current_profile,
			sizeof(s_saved_game_cartographer_player_profile_v1));
	}

	input_abstraction_set_controller_look_sensitivity(controller_index, current_profile->controller_sensitivity);
	input_abstraction_set_controller_thumb_deadzone(controller_index);
}

void cartographer_player_profile_save(e_controller_index controller_index)
{
	if (g_cartographer_profile_file_index_map[controller_index] != NONE)
		saved_games_async_helper_write_saved_game_bin(cartographer_bin,
			g_cartographer_profile_file_index_map[controller_index],
			(int8*)&g_cartographer_profile_settings[controller_index],
			sizeof(s_saved_game_cartographer_player_profile_v1));
}

void cartographer_player_profile_initialize()
{
	for (uint32 index = 0; index < k_number_of_controllers; ++index)
	{
		saved_games_cartographer_player_profile_v1_new(&g_cartographer_profile_settings[index]);
		g_cartographer_profile_file_index_map[index] = -1;
	}
}


