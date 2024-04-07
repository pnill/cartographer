#pragma once

#include "controllers.h"
#include "input_windows.h"

#define K_MAXIMUM_NUMBER_OF_GAME_FUNCTION_BINDS 8

enum e_input_preference_device_type :uint32
{
	_input_preference_device_general = 0xFFFFFFFF,
	_input_preference_device_mouse = 0x0,
	_input_preference_device_keyboard = 0x1,
	_input_preference_device_gamepad = 0x2,
};

enum e_button_functions
{
	_button_jump = 0x0,
	_button_trick = 0x1,
	_button_brake = 0x2,
	_button_switch_grenade = 0x3,
	_button_switch_weapon = 0x4,
	_button_melee_attack = 0x5,
	_button_flashlight = 0x6,
	_button_throw_grenade = 0x7,
	_button_speed_boost = 0x8,
	_button_e_brake = 0x9,
	_button_fire = 0xA,
	_button_start = 0xB,
	_button_back = 0xC,
	_button_crouch = 0xD,
	_button_scope_zoom = 0xE,
	_button_scope_zoom_in = 0xF,
	_button_scope_zoom_out = 0x10,
	_button_lean_left = 0x11,
	_button_lean_right = 0x12,
	_button_accept = 0x13,
	_button_cancel = 0x14,
	_button_banshee_bomb = 0x15,
	_button_dual_wield_primary_fire = 0x16,
	_button_dual_wield_secondary_fire = 0x17,
	_button_vehicle_primary_fire = 0x18,
	_button_vehicle_secondary_fire = 0x19,
	_button_text_chat_toggle = 0x1A,
	_button_text_chat_team = 0x1B,
	_button_ui_scroll_up = 0x1C,
	_button_ui_scroll_down = 0x1D,
	_button_team_voice = 0x1E,
	_button_flip_vehicle = 0x1F,
	_button_touch_device = 0x20,
	_button_enter_vehicle = 0x21,
	_button_board_vehicle = 0x22,
	_button_evict_from_vehicle = 0x23,
	_button_trade_weapon = 0x24,
	_button_pick_up_primary_weapon = 0x25,
	_button_pick_up_primary_multiplayer_weapon = 0x26,
	_button_pick_up_secondary_weapon = 0x27,
	_button_pick_up_secondary_multiplayer_weapon = 0x28,
	_button_put_away_secondary_weapon = 0x29,
	_button_put_away_or_drop_secondary_weapon = 0x2A,
	_button_reload = 0x2B,
	_button_exit_vehicle = 0x2C,
	_button_move_forward = 0x2D,
	_button_move_backward = 0x2E,
	_button_strafe_left = 0x2F,
	_button_strafe_right = 0x30,
	_button_mouse_yaw_left = 0x31,
	_button_mouse_yaw_right = 0x32,
	_button_mouse_pitch_forward = 0x33,
	_button_mouse_pitch_backward = 0x34,
	_extended_button_gamepad_yaw_left = 0x35,
	_extended_button_gamepad_yaw_right = 0x36,
	_extended_button_gamepad_pitch_forward = 0x37,
	_extended_button_gamepad_pitch_backward = 0x38,

	NUMBER_OF_BUTTON_KEYS = 0x39,
	NUMBER_OF_ABSTRACT_BUTTONS = 0x2D,
};

#pragma pack(push,1)
struct s_game_function_bind
{
	e_input_preference_device_type m_device_type;
	uint32 m_button_key;
	uint32 unknown;
};
CHECK_STRUCT_SIZE(s_game_function_bind, 0xC);

struct s_game_function
{
	uint32 m_bind_count;
	s_game_function_bind m_bind[K_MAXIMUM_NUMBER_OF_GAME_FUNCTION_BINDS];
};
CHECK_STRUCT_SIZE(s_game_function, 0x64);


struct s_gamepad_input_preferences
{
	real32 mouse_yaw_rate;
	real32 mouse_pitch_rate;
	real32 gamepad_yaw_rate;
	real32 gamepad_pitch_rate;
	bool gamepad_invert_look;
	bool mouse_invert_look;
	bool invert_aircraft_control;
	uint8 gap_13;
	s_game_function game_function_mapping[NUMBER_OF_BUTTON_KEYS];
	uint32 field_1658;
	real32 binary_yaw_rate;
	real32 binary_pitch_rate;
	real32 stick_threshold;
	real32 mouse_delta_threshold;
	real32 mouse_wheel_threshold;
	bool invert_dual_wield;
	uint8 gap_1671[3];
	real32 mouse_acceleration;
	int16 gamepad_axial_deadzone_left_x;
	int16 gamepad_axial_deadzone_left_y;
	int16 gamepad_axial_deadzone_right_x;
	int16 gamepad_axial_deadzone_right_y;
};
CHECK_STRUCT_SIZE(s_gamepad_input_preferences, 0x1680);

struct s_keyboard_input_preferences
{
	s_key_state keys[NUMBER_OF_BUTTON_KEYS];
};
CHECK_STRUCT_SIZE(s_keyboard_input_preferences, 0x1C8);

// TODO : verify this struct and NUMBER_OF_ABSTRACT_BUTTONS
struct s_game_input_state
{
	uint8 m_down_frames[NUMBER_OF_ABSTRACT_BUTTONS];
	uint8 pad2E;
	uint16 m_down_msec[NUMBER_OF_ABSTRACT_BUTTONS];
	real32 primary_fire;
	real32 secondary_fire;
	real32 forward_movement;
	real32 strafe;
	real_euler_angles2d mouse;
	real_euler_angles2d gamepad;
	real32 mouse_pitch2;
	real32 gamepad_pitch2;
	uint32 field_B0;
	uint32 field_B4;
};
CHECK_STRUCT_SIZE(s_game_input_state, 0xB8);


struct s_input_abstraction_globals
{
	s_gamepad_input_preferences preferences[k_number_of_controllers];
	s_keyboard_input_preferences keyboard_preferences;
	s_game_input_state input_states[k_number_of_controllers];
	uint32 controller_detection_timer;
	bool input_has_gamepad[k_number_of_controllers];
	bool input_device_changed;
	uint8 gap_5EB1[7];
};
CHECK_STRUCT_SIZE(s_input_abstraction_globals, 0x5EB8);
#pragma pack(pop)


extern s_input_abstraction_globals* input_abstraction_globals;


void __cdecl input_abstraction_initialize();
void __cdecl input_abstraction_dispose();
void __cdecl input_abstraction_handle_device_change(uint32 flags);
void __cdecl input_abstraction_get_controller_preferences(e_controller_index controller_index, s_gamepad_input_preferences* preferences);
void __cdecl input_abstraction_get_input_state(e_controller_index controller_index, s_game_input_state* state);
void __cdecl input_abstraction_get_player_look_angular_velocity(e_controller_index controller_index, real_euler_angles2d* angular_velocity);
void __cdecl input_abstraction_get_player_look_angular_velocity_for_mouse(e_controller_index controller_index, real_euler_angles2d* angular_velocity);
bool __cdecl input_abstraction_controller_button_test(e_controller_index controller_index, e_button_functions button_index);
e_button_functions __cdecl input_abstraction_get_primary_fire_button(datum unit);
e_button_functions __cdecl input_abstraction_get_secondary_fire_button(datum unit);

bool __cdecl input_abstraction_get_key_state(int16 key);

void input_abstraction_patches_apply();
