#pragma once
#include "cartographer_player_profile_weapon_offsets.h"

#include "input/controllers.h"
#include "H2MOD/Modules/Input/ControllerInput.h"

enum e_saved_game_cartographer_player_profile_version : uint32
{
	k_saved_game_cartographer_player_profile_version_invalid = 0,
	_saved_game_cartographer_player_profile_version_1,
	_saved_game_cartographer_player_profile_version_2,
	_saved_game_cartographer_player_profile_version_3,

	k_saved_game_cartographer_player_profile_version_count,
};

enum e_cartographer_controller_deadzone_type : uint8 
{
	_controller_deadzone_type_axial,
	_controller_deadzone_type_radial,
	_controller_deadzone_type_combined
};

/* structures */

// THIS SHOULD NEVER CHANGE
// ALWAYS INCLUDE THIS IN A PROFILE
struct s_saved_game_cartographer_player_profile_main_header
{
	e_saved_game_cartographer_player_profile_version version;
	uint32 signature;
};
ASSERT_STRUCT_SIZE(s_saved_game_cartographer_player_profile_main_header, 8);

struct s_saved_game_cartographer_player_profile_v1
{
	s_saved_game_cartographer_player_profile_main_header header;
};
ASSERT_STRUCT_SIZE(s_saved_game_cartographer_player_profile_v1, 8);

struct s_saved_game_cartographer_player_profile_v2
{
	s_saved_game_cartographer_player_profile_main_header header;
	uint8 field_of_view;
	uint8 vehicle_field_of_view;
	bool static_first_person;
	uint8 pad_0[1];
	real32 mouse_sensitivity;
	real32 raw_mouse_sensitivity;
	bool mouse_uniform;
	bool raw_mouse_input;
	uint8 pad_1[2];
	real32 controller_sensitivity;
	bool controller_modern;
	int8 controller_deadzone_type;
	uint8 pad_2[2];
	ControllerInput::CustomControllerLayout custom_layout;
	real_point2d deadzone_axial;
	real32 deadzone_radial;
	real32 crosshair_offset;
	real32 crosshair_scale;
};
ASSERT_STRUCT_SIZE(s_saved_game_cartographer_player_profile_v2, 80);

struct s_saved_game_cartographer_player_profile_v3
{
	s_saved_game_cartographer_player_profile_main_header header;
	uint8 field_of_view;
	uint8 vehicle_field_of_view;
	bool static_first_person;
	bool controller_custom_layout_enabled;
	real32 mouse_sensitivity;
	real32 raw_mouse_sensitivity;
	bool mouse_uniform;
	bool raw_mouse_input;
	bool controller_modern;
	int8 controller_deadzone_type;
	real32 controller_sensitivity;
	ControllerInput::CustomControllerLayout custom_layout;
	real_point2d deadzone_axial;
	real32 deadzone_radial;
	real32 crosshair_offset;
	real32 crosshair_scale;
	real_point3d weapon_offsets[k_weapon_offset_weapon_count];
};
ASSERT_STRUCT_SIZE(s_saved_game_cartographer_player_profile_v3, 268);


// These properties control the currently used version of the cartographer profile in the project.
typedef s_saved_game_cartographer_player_profile_v3 s_saved_game_cartographer_player_profile;

union s_saved_game_cartographer_player_profile_container
{
	s_saved_game_cartographer_player_profile_v1 v1;
	s_saved_game_cartographer_player_profile_v2 v2;
	s_saved_game_cartographer_player_profile_v3 v3;
	s_saved_game_cartographer_player_profile current;
};

struct s_cartographer_profile_run_time
{
	s_saved_game_cartographer_player_profile_container profile;
	int32 enumerated_file_index;
	e_controller_index controller_index;
	int32 user_index;
	uint32 gamepad_index;
};

/* constants */

constexpr e_saved_game_cartographer_player_profile_version k_cartographer_profile_version = _saved_game_cartographer_player_profile_version_3;

/* prototype */

void cartographer_player_profile_initialize(void);

bool cartographer_player_profile_is_signed_in(e_controller_index controller_index);

s_saved_game_cartographer_player_profile* cartographer_player_profile_get_by_controller_index(e_controller_index controller_index);

s_saved_game_cartographer_player_profile* cartographer_player_profile_get_by_user_index(int32 user_index);

void cartographer_player_profile_sign_in(e_controller_index controller_index, enumerated_file_index enumerated_file_index);

void cartographer_player_profile_sign_out(e_controller_index controller_index);

void cartographer_player_profile_save(e_controller_index controller_index);
