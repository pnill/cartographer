#pragma once
#include "H2MOD/Modules/Input/ControllerInput.h"
#include "input/controllers.h"

enum e_saved_game_cartographer_player_profile_version : uint32
{
	_saved_game_cartographer_player_profile_version_1 = 0x1,
	_saved_game_cartographer_player_profile_version_2 = 0x2,

	k_saved_game_cartographer_player_profile_version_count,
	k_saved_game_cartographer_player_profile_version_invalid = -1
};

struct s_saved_game_cartographer_player_profile_v1
{
	e_saved_game_cartographer_player_profile_version version;
	uint32 signature;
};

struct s_saved_game_cartographer_player_profile_v2
{
	e_saved_game_cartographer_player_profile_version version;
	uint32 signature;
	real32 field_of_view;
	uint32 vehicle_field_of_view;
	bool static_first_person;
	real32 mouse_sensitivity;
	real32 raw_mouse_sensitivity;
	bool mouse_uniform;
	bool raw_mouse_input;
	real32 controller_sensitivity;
	bool controller_modern;
	int8 controller_deadzone_type;
	ControllerInput::CustomControllerLayout custom_layout;
	real32 deadzone_axial_x;
	real32 deadzone_axial_y;
	real32 deadzone_radial;
	real32 crosshair_offset;
	real32 crosshair_scale;
};

// These properties control the currently used version of the cartographer profile in the project.
typedef s_saved_game_cartographer_player_profile_v2 s_saved_game_cartographer_player_profile;

constexpr e_saved_game_cartographer_player_profile_version g_cartographer_profile_version = _saved_game_cartographer_player_profile_version_2;


struct s_cartographer_profile_run_time
{
	s_saved_game_cartographer_player_profile profile;
	uint32 enumerated_file_index;
	e_controller_index controller_index;
	uint32 user_index;
	uint32 gamepad_index;
};

s_saved_game_cartographer_player_profile* cartographer_player_profile_get_by_controller_index(e_controller_index controller_index);

s_saved_game_cartographer_player_profile* cartographer_player_profile_get_by_user_index(uint32 user_index);

void cartographer_player_profile_sign_in(e_controller_index controller_index, uint32 enumerated_file_index);

void cartographer_player_profile_sign_out(e_controller_index controller_index);

void cartographer_player_profile_save(e_controller_index controller_index);

void cartographer_player_profile_initialize();