#pragma once
#include "H2MOD/Modules/Input/ControllerInput.h"
#include "input/controllers.h"

enum e_saved_game_cartographer_player_profile_version : uint32
{
	_saved_game_cartographer_player_profile_version_1 = 0x1,
	_saved_game_cartographer_player_profile_version_2 = 0x2,
};

struct s_saved_game_cartographer_player_profile_v1
{
	e_saved_game_cartographer_player_profile_version version;
	int32 field_of_view;
	int32 vehicle_field_of_view;
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

void cartographer_player_profile_v1_new(s_saved_game_cartographer_player_profile_v1* settings);

s_saved_game_cartographer_player_profile_v1* cartographer_player_profile_get(e_controller_index controller_index);

void cartographer_player_profile_load(e_controller_index controller_index, uint32 enumerated_file_index);

void cartographer_player_profile_save(e_controller_index controller_index);

void cartographer_player_profile_initialize();