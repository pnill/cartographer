#pragma once

#include "input/controllers.h"
#include "saved_games/player_profile.h"

enum e_user_interface_event_type : uint32
{
	_user_interface_event_type0 = 0x0,
	_user_interface_event_type_tab_up = 0x1,
	_user_interface_event_type_tab_left = 0x2,
	_user_interface_event_type_tab_down = 0x3,
	_user_interface_event_type_tab_right = 0x4,
	_user_interface_event_type_gamepad_button_pressed = 0x5,
	_user_interface_event_type_mouse_button_left_click = 0x6,
	_user_interface_event_type_keyboard_button_pressed = 0x7,
	_user_interface_event_type8 = 0x8,
	_user_interface_event_type9 = 0x9,
	_user_interface_event_type_mouse_scroll_up = 0xA,
	_user_interface_event_type_mouse_scroll_down = 0xB,
	_user_interface_event_type_automation = 0xC,
	_user_interface_event_type13 = 0xD,
	_user_interface_event_type14 = 0xE,
	_user_interface_event_type15 = 0xF,
};

// TODO : recheck these
enum e_user_interface_controller_component : uint32
{
	_user_interface_controller_component_button_a = 0x0,
	_user_interface_controller_component_button_b = 0x1,
	_user_interface_controller_component_button_x = 0x2,
	_user_interface_controller_component_button_y = 0x3,
	_user_interface_controller_component_button_left_bumper = 0x4,
	_user_interface_controller_component_button_right_bumper = 0x5,
	_user_interface_controller_component_button_left_trigger = 0x6,
	_user_interface_controller_component_button_right_trigger = 0x7,
	_user_interface_controller_component_button_start = 0xC,
	_user_interface_controller_component_button_back = 0xD,
	_user_interface_controller_component_button_left_stick = 0xE,
	_user_interface_controller_component_button_right_stick = 0xF,
	_user_interface_controller_component_any_stick = 0x10,



	// keyboard_component are similar to Virtual KeyCodes (VK_KEY)
	_user_interface_keyboard_component_button_tab = 0x9,
	_user_interface_keyboard_component_button_backspace = 0x8,
	_user_interface_keyboard_component_button_enter = 0xD,
	_user_interface_keyboard_component_button_pause_break = 0x13,
	_user_interface_keyboard_component_button_capslock = 0x14,
	_user_interface_keyboard_component_button_escape = 0x1B,
	_user_interface_keyboard_component_buton_space = 0x20,
	_user_interface_keyboard_component_buton_page_up = 0x21,
	_user_interface_keyboard_component_buton_page_down = 0x22,
	_user_interface_keyboard_component_buton_end = 0x23,

	_user_interface_keyboard_component_buton_left = 0x25,
	_user_interface_keyboard_component_buton_up = 0x26,
	_user_interface_keyboard_component_buton_right = 0x27,
	_user_interface_keyboard_component_buton_down = 0x28,
	_user_interface_keyboard_component_button_insert = 0x2D,
	_user_interface_keyboard_component_button_delete = 0x2E,


	_user_interface_keyboard_component_buton_numeric_zero = 0x30,
	_user_interface_keyboard_component_buton_numeric_two = 0x32,
	// ....so on
	_user_interface_keyboard_component_buton_numeric_nine = 0x39,


	_user_interface_keyboard_component_buton_letter_a = 0x41,
	_user_interface_keyboard_component_buton_letter_b = 0x42,
	// ....so on
	_user_interface_keyboard_component_buton_letter_z = 0x5A,


	_user_interface_keyboard_component_buton_f1 = 0x70,
	_user_interface_keyboard_component_buton_f2 = 0x71,
	// ....so on
	_user_interface_keyboard_component_buton_f12 = 0x7B,



	// ....special keys
	_user_interface_keyboard_component_buton_numlock = 0x90,
	_user_interface_keyboard_component_buton_left_shift = 0xA0,
	_user_interface_keyboard_component_buton_right_shift = 0xA1,
	_user_interface_keyboard_component_buton_left_control = 0xA2,
	_user_interface_keyboard_component_buton_right_control = 0xA3,
	_user_interface_keyboard_component_buton_left_alt = 0xA4,
	_user_interface_keyboard_component_buton_right_alt = 0xA5,
	_user_interface_keyboard_component_buton_tilde = 0x107,
	_user_interface_keyboard_component_buton_minus = 0x108,
	_user_interface_keyboard_component_buton_comma = 0x10E,
	_user_interface_keyboard_component_buton_period = 0x10E,
	_user_interface_keyboard_component_buton_backslash = 0x110,
	_user_interface_keyboard_component_buton_equals = 0x112,
	_user_interface_keyboard_component_buton_multiple = 0x115,
	_user_interface_keyboard_component_buton_numeric_one = 0x117,
};


enum e_user_interface_automation_mode : uint16
{
	_user_interface_automation_mode_none = 0x0,
	_user_interface_automation_mode_campaign_single_player = 0x1,
	_user_interface_automation_mode_splitscreen_multiplayer = 0x2,
	_user_interface_automation_mode_system_link_auto = 0x3,
	_user_interface_automation_mode_system_link_auto_host = 0x4,
	_user_interface_automation_mode_system_link_auto_join = 0x5,
	_user_interface_automation_mode_system_link_manual = 0x6,
	_user_interface_automation_mode_xbox_live_auto = 0x7,
	_user_interface_automation_mode_xbox_live_auto_host = 0x8,
	_user_interface_automation_mode_xbox_live_auto_join = 0x9,
	_user_interface_automation_mode_xbox_live_manual = 0xA,
	_user_interface_automation_mode_matchmaking = 0xB,
	k_user_interface_automation_modes_count = 0xC,
};

// Credits :  https://github.com/twist84/ManagedDonkey/blob/main/game/source/interface/user_interface_controller.hpp
#pragma pack(push , 1)
struct s_event_record
{
	e_user_interface_event_type type;
	e_controller_index controller;
	e_user_interface_controller_component component;	
	uint16 value; // // holds e_user_interface_automation_mode during _user_interface_event_type_automation
	PAD(2);
};
#pragma pack(pop)
ASSERT_STRUCT_SIZE(s_event_record, 16);

enum e_controller_state_flags
{
	_controller_state_bit0,
	_controller_state_bit1,
	_controller_state_bit2,
	_controller_state_has_signed_in_bit = 3,
	_controller_state_has_xbox_live_bit = 4,
	_controller_state_save_in_progress_bit = 5,
	_controller_state_bit6,
	k_controller_state_flags_count,
};

struct s_user_interface_controller
{
	c_flags<e_controller_state_flags, uint32, k_controller_state_flags_count> m_flags;
	uint32 user_index;
	s_player_identifier controller_user_identifier;
	PAD32;
	s_saved_game_file_player_profile player_profile;
	uint32 profile_index;
	e_game_team player_team;
	PAD16;
	e_handicap player_handicap_level;
	PAD(3);
	int8 bungienet_user;
	PAD(3);
	int8 player_is_griefer;
	PAD(3);
	int8 achievement_flags;
	PAD(3);
	s_player_identifier field_1234;
	c_static_wchar_string32 player_name;
};
ASSERT_STRUCT_SIZE(s_user_interface_controller, 4732);

struct s_user_interface_controller_globals
{
	s_user_interface_controller controllers[k_number_of_controllers];
	s_event_record event_records[k_number_of_controllers];
	bool controller_detached[k_number_of_controllers];
	bool event_manager_suppress;
	PAD(3);
};
ASSERT_STRUCT_SIZE(s_user_interface_controller_globals, 19000);

s_user_interface_controller_globals* user_interface_controller_globals_get(void);

void __cdecl user_interface_controller_initialize();
void __cdecl user_interface_controller_update();
bool __cdecl user_interface_controller_is_player_profile_valid(e_controller_index controller_index);
uint32 __cdecl user_interface_controller_get_next_valid_index(e_controller_index controller_index);
uint32 __cdecl user_interface_controller_get_user_index(e_controller_index controller_index);
void __cdecl user_interface_controller_set_user_index(e_controller_index controller_index, uint32 user_index);
e_controller_index __cdecl user_interface_controller_get_controller_for_user(uint32 user_index);
e_game_team __cdecl user_interface_controller_get_user_active_team(e_controller_index controller_index);
void __cdecl user_interface_controller_set_desired_team_index(e_controller_index controller_index, e_game_team team);
bool __cdecl user_interface_controller_get_rumble_enabled(e_controller_index controller_index);
bool __cdecl user_interface_controller_get_autolevel_enabled(e_controller_index controller_index);
e_handicap __cdecl user_interface_controller_get_user_handicap_level(e_controller_index controller_index);
void __cdecl user_interface_controller_set_user_handicap_level(e_controller_index controller_index, e_handicap handicap);
void __cdecl user_interface_controller_set_griefer(e_controller_index controller_index, bool griefing);
wchar_t* __cdecl user_interface_controller_get_player_profile_name(e_controller_index controller_index);

uint32 __cdecl user_interface_controller_get_signed_in_controller_count();
uint32 __cdecl user_interface_controller_get_signed_in_controllers_mask();
uint32 __cdecl user_interface_controller_get_last_level_played(e_controller_index controller_index);
uint32 __cdecl user_interface_controller_get_highest_campaign_level_in_signed_in_controllers();
void __cdecl user_interface_controller_sign_out(e_controller_index controller_index);
void __cdecl user_interface_controller_sign_out_all_controllers();
void user_interface_controller_get_profile_data(e_controller_index controller_index, s_saved_game_file_player_profile* profile, uint32* profile_index);
void __cdecl user_interface_controller_get_user_properties(e_controller_index controller_index, s_player_identifier* controller_user_identifier, s_player_properties* properties);
void __cdecl user_interface_controller_event_submit(s_event_record* event);

void __cdecl user_interface_controller_update_network_properties(e_controller_index controller_index);


void user_inteface_controller_apply_patches();
