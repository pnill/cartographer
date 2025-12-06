#include "stdafx.h"
#include "input_windows.h"

#include "controllers.h"
#include "input_abstraction.h"
#include "input_xinput.h"

#include "interface/user_interface_controller.h"
#include "main/main_time.h"
#include "main/game_preferences.h"
#include "shell/shell_windows.h"

#include "H2MOD/Modules/Shell/Config.h"

/* constants */

real32 g_rumble_factor = 1.f;

/* structures */

struct s_key_remap
{
	const e_input_key_code key;
	const WCHAR ch;
	int32 virtual_key;
};

/* prototypes */

static void input_stop_removed_controller_handler_from_panicking(void);

static void input_rewrite_button_timing_loop(void);

static e_input_key_code input_map_ascii_to_keycode(uint8 ascii);

static void __cdecl update_button(uint8* frames, uint16* msec, bool* key_bool, bool down, int32 elapsed_msec);

// Hook for update_button call in input_update
static void __cdecl update_button_input_update_hook(uint8 * frames, uint16 * msec, bool* key_bool, bool down, int32 elapsed_msec);

static void update_button_release_key(e_input_key_code code);

static int compare_device_compatibility(const void* p1, const void* p2);

static int compare_device_ports(const void* p1, const void* p2);

static void input_windows_update_device_mapping(void);

static void input_windows_restore_device_mapping(void);

static bool input_has_gamepad_hook(int16 gamepad_index, bool* a1);

/* globals */

// This global stores a list of key codes as well as the associated characters they need to press
// Virtual keys are resolved at runtime by checking the keyboard layout for this specific character key
static s_key_remap g_key_remap[REMAPPED_KEY_COUNT] =
{
	{ _key_tilde, L'`', NONE },
	{ _key_subtract, L'-', NONE },
	{ _key_left_square_bracket, L'[', NONE },
	{ _key_right_square_bracket, L']', NONE },
	{ _key_backslash, L'\\', NONE },
	{ _key_semicolon, L';', NONE },
	{ _key_apostrophe, L'\'', NONE },
	{ _key_comma, L',', NONE },
	{ _key_period, L'.', NONE },
	{ _key_forwardslash, L'/', NONE },
	{ _key_equal, L'=', NONE },
	{ _key_add, L'+', NONE },
	{ _key_circumflex_accent, L'^', NONE },
	{ _key_dollar_sign, L'$', NONE },
	{ _key_asterisk, L'*', NONE },
	{ _key_colon, L':', NONE },
	{ _key_exclamation_mark, L'!', NONE },
	{ _key_ampersand, L'&', NONE },
	{ _key_quotation_mark, L'\"', NONE },
	{ _key_u_umlaut, L'\xFC', NONE },
	{ _key_o_umlaut, L'\xF6', NONE },
	{ _key_a_umalut, L'\xE4', NONE },
	{ _key_eszett, L'\xDF', NONE },
	{ _key_u_accent_grave, L'\xF9', NONE },
	{ _key_ene, L'\xF1', NONE },
	{ _key_acute_accent, L'\xB4', NONE },
	{ _key_e_grave, L'\xE8', NONE },
	{ _key_o_grave, L'\xF2', NONE },
	{ _key_a_grave, L'\xE0', NONE },
	{ _key_c_cedilla, L'\xE7', NONE }
};

static c_static_flags_no_init<NUMBER_OF_KEYS> g_keyboard_input_state;

static bool g_should_offset_gamepad_indices = false;

static bool g_notified_to_change_mapping = false;

ascii_key ascii_to_key_table[NUMBER_OF_KEYS] =
{
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_backspace, true, _string_id_key_backspace, L'\0' },
	{ _key_tab, true, _string_id_key_tab, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_return, true, _string_id_key_return, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _keypad_left_shift, true, _string_id_key_left_shift, L'\0' },
	{ _keypad_left_control, true, _string_id_key_left_control, L'\0' },
	{ _key_left_alt, true, _string_id_key_left_alt, L'\0' },
	{ _key_pause, true, _string_id_key_pause, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_escape, true, _string_id_key_escape, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_space, true, _string_id_key_space, L'\0' },
	{ _key_page_up, true, _string_id_key_page_up, L'\0' },
	{ _key_page_down, true, _string_id_key_page_down, L'\0' },
	{ _key_end, true, _string_id_key_end, L'\0' },
	{ _key_home, true, _string_id_key_home, L'\0' },
	{ _key_left_arrow, true, _string_id_key_left_arrow, L'\0' },
	{ _key_up_arrow, true, _string_id_key_up_arrow, L'\0' },
	{ _key_right_arrow, true, _string_id_key_right_arrow, L'\0' },
	{ _key_down_arrow, true, _string_id_key_down_arrow, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_snapshot, true, _string_id_key_print_screen, L'\0' },
	{ _key_insert, true, _string_id_key_insert, L'\0' },
	{ _key_delete, true, _string_id_key_delete, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_left_windows, true, _string_id_key_left_windows, L'\0' },
	{ _key_right_windows, true, _string_id_key_right_windows, L'\0' },
	{ _key_application, true, _string_id_key_menu, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _keypad_0, true, _string_id_keypad_0, L'\0' },
	{ _keypad_1, true, _string_id_keypad_1, L'\0' },
	{ _keypad_2, true, _string_id_keypad_2, L'\0' },
	{ _keypad_3, true, _string_id_keypad_3, L'\0' },
	{ _keypad_4, true, _string_id_keypad_4, L'\0' },
	{ _keypad_5, true, _string_id_keypad_5, L'\0' },
	{ _keypad_6, true, _string_id_keypad_6, L'\0' },
	{ _keypad_7, true, _string_id_keypad_7, L'\0' },
	{ _keypad_8, true, _string_id_keypad_8, L'\0' },
	{ _keypad_9, true, _string_id_keypad_9, L'\0' },
	{ _keypad_multiply, true, _string_id_keypad_multiply, L'\0' },
	{ _keypad_add, true, _string_id_keypad_add, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _keypad_subtract, true, _string_id_keypad_subtract, L'\0' },
	{ _keypad_decimal, true, _string_id_keypad_decimal, L'\0' },
	{ _keypad_divide, true, _string_id_keypad_divide, L'\0' },
	{ _key_f1, true, _string_id_key_f1, L'\0' },
	{ _key_f2, true, _string_id_key_f2, L'\0' },
	{ _key_f3, true, _string_id_key_f3, L'\0' },
	{ _key_f4, true, _string_id_key_f4, L'\0' },
	{ _key_f5, true, _string_id_key_f5, L'\0' },
	{ _key_f6, true, _string_id_key_f6, L'\0' },
	{ _key_f7, true, _string_id_key_f7, L'\0' },
	{ _key_f8, true, _string_id_key_f8, L'\0' },
	{ _key_f9, true, _string_id_key_f9, L'\0' },
	{ _key_f10, true, _string_id_key_f10, L'\0' },
	{ _key_f11, true, _string_id_key_f11, L'\0' },
	{ _key_f12, true, _string_id_key_f12, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _keypad_num_lock, true, _string_id_keypad_num_lock, L'\0' },
	{ _key_scroll_lock, true, _string_id_key_scroll_lock, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_left_shift, true, _string_id_key_left_shift, L'\0' },
	{ _key_right_shift, true, _string_id_key_right_shift, L'\0' },
	{ _key_left_control, true, _string_id_key_left_control, L'\0' },
	{ _key_right_control, true, _string_id_key_right_control, L'\0' },
	{ _key_left_alt, true, _string_id_key_left_alt, L'\0' },
	{ _key_right_alt, true, _string_id_key_right_alt, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' },
	{ _key_not_a_key, false, _string_id_invalid, L'\0' }
};

input_globals_windows* input_globals;

bool* g_input_windows_request_terminate;

uint32 input_device_change_delay_timer = 0;

/* public code */

void input_windows_apply_patches(void)
{
	input_globals = Memory::GetAddress<input_globals_windows*>(0x479F50);
	g_input_windows_request_terminate = Memory::GetAddress<bool*>(0x971291);

	PatchCall(Memory::GetAddress(0x9020F), input_set_gamepad_rumbler_state);    // Replace call in rumble_clear_all_now

	PatchCall(Memory::GetAddress(0x2FA62), input_update_main_device_state);		// Replace call in input_windows_update
	PatchCall(Memory::GetAddress(0x2FC2F), input_update_main_device_state);		// Replace call in input_windows_update
	PatchCall(Memory::GetAddress(0x2FBD2), input_update_gamepads);				// Replace call in input_windows_update

	input_stop_removed_controller_handler_from_panicking();
	
	// Replace initialize key mapping function so we properly handle keyboard layouts
	PatchCall(Memory::GetAddress(0x2FEA9), input_windows_initialize_key_remapping);
	PatchCall(Memory::GetAddress(0x2EFF5), input_windows_get_remapped_key);
	PatchCall(Memory::GetAddress(0x2F0CF), input_windows_get_remapped_key);
	PatchCall(Memory::GetAddress(0x7C87), input_add_key);

	// Add logic to take the config option to disable the keyboard into account
	PatchCall(Memory::GetAddress(0x2FAAB), update_button);

	PatchCall(Memory::GetAddress(0x621C5), input_get_mouse_state);
	PatchCall(Memory::GetAddress(0x20780B), input_get_mouse_state);

	input_rewrite_button_timing_loop();
	return;
}

void __cdecl input_initialize(void)
{
	INVOKE(0x2FD23, 0x0, input_initialize);
	return;
}

void __cdecl input_dispose(void)
{
	INVOKE(0x2E309, 0x0, input_dispose);
	return;
}

void __cdecl input_windows_update(void)
{
	INVOKE(0x2F9AC, 0x0, input_windows_update);
	return;
}

void input_suppress(void)
{
	input_globals->input_suppressed = true;
	return;
}

void input_add_key(int32 msg, uint32 wParam, uint32 lParam, bool fHandled)
{
	if (input_globals->active_flag)
	{
		bool upper_bit_exists = false;

		uint8 key = _key_not_a_key;
		key_stroke keystroke;
		keystroke.modifier_flags = 0;
		keystroke.ascii_code = NONE;
		keystroke.key_code = _key_not_a_key;

		// Handle system keys
		if (msg == WM_KEYFIRST || msg == WM_SYSKEYDOWN)
		{
			ASSERT(VALID_INDEX(wParam, NUMBER_OF_VIRTUAL_CODES));
			
			key = (uint8)wParam;
			keystroke.ascii_code = NONE;
			keystroke.utf16_code = (wchar_t)NONE;

			// Sapien code?
			if (get_current_language() == _language_german && GetKeyState(VK_CONTROL))
			{
				GetKeyState(VK_LMENU);
			}

			switch (key)
			{
			case _key_shift:
				if (GetKeyState(VK_RSHIFT))
				{
					key = VK_RSHIFT;
				}
				else if (GetKeyState(VK_LSHIFT))
				{
					key = VK_LSHIFT;
				}
				break;
			case _key_control:
				if (GetKeyState(VK_LCONTROL))
				{
					key = VK_LCONTROL;
				}
				else if (GetKeyState(VK_RCONTROL))
				{
					key = VK_RCONTROL;
				}
				break;
			case _key_menu:
				if (GetKeyState(VK_RMENU))
				{
					key = VK_RMENU;
				}
				else if (GetKeyState(VK_LMENU))
				{
					key = VK_LMENU;
				}
				break;
			}
			keystroke.key_code = input_map_ascii_to_keycode(key);
			g_keyboard_input_state.set(key, true);
		}
		// Handle other characters
		else if (msg == WM_CHAR || msg == WM_SYSCHAR)
		{
			const HKL layout = GetKeyboardLayout(0);

			key = (uint8)(VkKeyScanExW((WCHAR)wParam, layout) & BYTE_MAX);	// & BYTE_MAX since we ignore the upper bit shift state
			upper_bit_exists = (wParam & 0xFF00) != 0;

			if (upper_bit_exists || wParam >= VK_SPACE)
			{
				keystroke.ascii_code = (int8)(upper_bit_exists ? NONE : wParam);
				keystroke.utf16_code = (wchar_t)wParam;
				keystroke.key_code = input_map_ascii_to_keycode(key);
				g_keyboard_input_state.set(key, true);
			}
		}

		keystroke.repeating = TEST_BIT(lParam, 30);	// Set repeating to if bit 30 is true

		if (keystroke.key_code != _key_not_a_key || upper_bit_exists)
		{
			// If key is already handled and the key code isn't invalid then exit
			if (fHandled && keystroke.key_code != _key_not_a_key)
			{
				input_globals->keyboard.key_bool[key] = true;
				return;
			}

			// Set modifier flags on the key pressed
			SET_BIT(keystroke.modifier_flags, _key_modifier_shift_bit, GetKeyState(VK_SHIFT) < 0);
			SET_BIT(keystroke.modifier_flags, _key_modifier_control_bit, GetKeyState(VK_CONTROL) < 0);
			SET_BIT(keystroke.modifier_flags, _key_modifier_alt_bit, GetKeyState(VK_MENU) < 0);
			
			// Add keystroke to buffered keys
			if (input_globals->buffered_key_read_count < MAXIMUM_BUFFERED_KEYSTROKES)
			{
				input_globals->buffered_keys[input_globals->buffered_key_read_count] = keystroke;
				++input_globals->buffered_key_read_count;
			}

			// Adjust timing for key
			if (input_globals->keyboard.frames_down[key] == 0)
			{
				input_globals->keyboard.frames_down[key] = 1;
				input_globals->keyboard.msec_down[key] = 1;
			}
		}
	}

	// Clear the key state for system keys
	byte key_state[256];
	if (GetKeyboardState(key_state))
	{
		key_state[_key_left_shift] = 0;
		key_state[_key_right_shift] = 0;
		key_state[_key_left_control] = 0;
		key_state[_key_right_control] = 0;
		key_state[_key_left_alt] = 0;
		key_state[_key_right_alt] = 0;
		SetKeyboardState(key_state);
	}
	return;
}

void __cdecl input_update_gamepads(uint32 duration_ms)
{
	//INVOKE(0x2E7A4, 0x0, input_update_gamepads, duration_ms);

	/*
	* Reimplemented function removes dependency on input_globals.main_controller_index
	* Actually Updates input_globals.gamepad_states[index] buffer from device state
	*/

	bool input_handled = false;
	for (uint16 gamepad_index = _controller_index_0; gamepad_index < k_number_of_controllers; gamepad_index++)
	{
		if (input_has_gamepad(gamepad_index, nullptr))
		{
			s_gamepad_input_button_state* gamepad_state = input_get_gamepad_state(gamepad_index);

			if (input_xinput_update_gamepad(gamepad_index, duration_ms, gamepad_state))
			{
				//handled successfully for any device
				input_handled = true;
			}
		}
	}

	const s_window_globals* window_globals = window_globals_get();

	if (input_handled
		&& window_globals->hWnd == GetFocus()
		&& window_globals->hWnd == GetForegroundWindow()
		&& !main_time_is_throttled())
	{
		if ((input_globals->field7D8 & 1) == 0)
		{
			input_globals->field7D8 |= 1u;
			//v26 = 0;
			input_globals->field7D0 = system_milliseconds();
			//v26 = 0xFFFFFFFF;
		}
		uint32 time = system_milliseconds();
		if (time - input_globals->field7D0 > 15000 || time - input_globals->field7D0 < 0)
		{
			input_globals->field7D0 = time;
			INPUT input;
			//csmemset(&input, 0, sizeof(input));
			input.type = INPUT_KEYBOARD;
			input.ki.wVk = 0;
			input.ki.wScan = 0;
			input.ki.dwFlags = KEYEVENTF_KEYUP;
			input.ki.time= 0;
			input.ki.dwExtraInfo = 0;
			SendInput(1, &input, sizeof(input));
		}
	}

	// we want device changes to happen smoothly, not abruptly
	if (g_notified_to_change_mapping && input_device_change_delay_timer > k_maximum_delay_for_split_inputs)
	{
		if (g_should_offset_gamepad_indices)
		{
			input_windows_update_device_mapping();
		}
		else
		{
			input_windows_restore_device_mapping();
		}
		g_notified_to_change_mapping = false;
		input_device_change_delay_timer = 0;
	}

	TEST_N_DEF(PC1);
}

// ### passes param through ECX
// ### THIS IS DEFINITELY NOT CDECL, nor thiscall, as the callee doesn't clear stack 
//void __cdecl input_update_mouse(DIMOUSESTATE2* mouse_state, uint32 duration_ms)
//{
//	INVOKE(0x2E60C, 0x0, input_update_mouse, mouse_state, duration_ms);
//}

bool __cdecl input_has_gamepad(uint16 gamepad_index, bool* a2)
{
	return INVOKE(0x2F3CD, 0x0, input_has_gamepad, gamepad_index, a2);
}

bool __cdecl input_has_gamepad_plugged(uint16 gamepad_index)
{
	//return INVOKE_TYPE(0x2E186, 0x0, bool(__cdecl*)(uint16), gamepad_index);
	return input_globals->gamepad_states[gamepad_index].connected;
}

bool __cdecl input_gamepad_just_left(uint16 gamepad_index)
{
	if (VALID_INDEX(gamepad_index, k_number_of_controllers))
	{
		return input_globals->gamepad_states[gamepad_index].m_device_just_left;
	}
	return false;
}

uint8 __cdecl input_get_connected_gamepads_count()
{
	uint8 count = 0;
	for (e_controller_index controller = first_controller();
		controller != k_no_controller;
		controller = next_controller(controller))
	{
		if (input_has_gamepad_plugged((uint16)controller))
		{
			++count;
		}
	}
	return count;
}

s_gamepad_input_state* input_get_gamepad(uint16 gamepad_index)
{
	return &input_globals->gamepad_states[gamepad_index];
}

s_gamepad_input_button_state* __cdecl input_get_gamepad_state(uint16 gamepad_index)
{
	return INVOKE(0x2F433, 0x0, input_get_gamepad_state, gamepad_index);
}

mouse_state* __cdecl input_get_mouse_state(void)
{
	// Only pass the mouse state if we have a valid dinput device and our mouse input is not being suppressed
	// Otherwise, pass it the suppressed state and ignore input
	const bool available = input_globals->mouse_dinput_device && !input_globals->input_suppressed;
	return available ? &input_globals->mouse : &input_globals->mouse_suppressed;
}

uint16* __cdecl input_get_mouse_button_state()
{
	if (!input_globals->input_suppressed)
		return input_globals->mouse_buttons;

	return nullptr;
}

bool input_peek_key(key_stroke* key)
{
	bool result = false;
	if (input_globals->buffered_key_read_index < input_globals->buffered_key_read_count)
	{
		ASSERT(VALID_INDEX(input_globals->buffered_key_read_index, MAXIMUM_BUFFERED_KEYSTROKES));
		*key = input_globals->buffered_keys[input_globals->buffered_key_read_index];
		result = true;
	}
	return result;
}

bool input_abstraction_get_key(key_stroke* key)
{
	const bool result = input_peek_key(key);
	if (result)
	{
		ASSERT(VALID_INDEX(input_globals->buffered_key_read_index, MAXIMUM_BUFFERED_KEYSTROKES));
		++input_globals->buffered_key_read_index;
	}
	return result;
}

void __cdecl input_update_main_device_state()
{
	//INVOKE(0x2E709, 0x0, input_update_main_device_state);

	/*
	* Reimplemented function removes dependency on input_globals.main_controller_index
	* Updates input_globals.gamepad_states[index].connected
	* Allows game to actually detect multiple controllers as multi inputs rather single input
	*/

	uint8 device_index = _controller_index_0;
	do
	{
		input_device* device = g_xinput_devices[device_index];
		XINPUT_STATE state;
		s_gamepad_input_state* gamepad = input_get_gamepad(device_index);
		uint32 error_code = ERROR_DEVICE_NOT_CONNECTED;


		if (!device
			|| (error_code = device->XGetState(&state)) == ERROR_SEVERITY_SUCCESS
			|| error_code == ERROR_DEVICE_NOT_CONNECTED)
		{

			bool dev_connected = gamepad->connected;
			bool success = error_code == ERROR_SEVERITY_SUCCESS;
			bool initially_not_connected = !gamepad->connected;
			bool dev_state_joined = initially_not_connected && success;
			bool dev_state_left = dev_connected && !success;

			gamepad->connected = success;
			gamepad->m_device_just_joined = dev_state_joined;
			gamepad->m_device_just_left = dev_state_left;
		}

		uint32 device_flags = 0;
		if (gamepad->m_device_just_left)
			SET_BIT(device_flags, 0, true);
		if (gamepad->m_device_just_joined)
			SET_BIT(device_flags, 13, true);

		input_abstraction_handle_device_change(device_flags);
		device_index++;

	} while (device_index < k_number_of_controllers);
}

void __cdecl input_set_gamepad_rumbler_state(int16 gamepad_index, uint16 left, uint16 right)
{
	ASSERT(VALID_INDEX(gamepad_index, k_number_of_controllers));

	XINPUT_VIBRATION state = { left, right };
	const XINPUT_VIBRATION state_none = { 0, 0 };

	state.wLeftMotorSpeed = (WORD)(state.wLeftMotorSpeed * g_rumble_factor);
	state.wRightMotorSpeed = (WORD)(state.wRightMotorSpeed * g_rumble_factor);

	bool enabled = user_interface_controller_get_rumble_enabled((e_controller_index)gamepad_index);
	input_globals->rumble_states[gamepad_index] = (enabled ? state : state_none);
	return;
}

uint8 __cdecl input_windows_key_frames_down(int16 key)
{
	return INVOKE(0x2EF86, 0x0, input_windows_key_frames_down, key);
}

uint16 __cdecl input_windows_key_msec_down(int16 key)
{
	return INVOKE(0x2F030, 0x0, input_windows_key_msec_down, key);
}

bool __cdecl input_windows_drive_letter_test(int32 memory_unit, int8* drive_letter)
{
	ASSERT(memory_unit < k_number_of_memory_units);
	return INVOKE(0x2E463, 0, input_windows_drive_letter_test, memory_unit, drive_letter);
}

bool input_windows_processing_device_change()
{
	return g_notified_to_change_mapping;
}

bool input_windows_has_split_device_active()
{
	return g_should_offset_gamepad_indices;
}

void input_windows_notify_change_device_mapping()
{
	g_notified_to_change_mapping = true;
	g_should_offset_gamepad_indices = !g_should_offset_gamepad_indices;
}

void input_windows_release_key(WCHAR param)
{
	ASSERT(VALID_INDEX(param, NUMBER_OF_KEYS));

	e_input_key_code code = (e_input_key_code)PIN(param, 0, _key_not_a_key);
	switch (code)
	{
	case _key_shift:
		if (GetAsyncKeyState(VK_LSHIFT) >= 0)
		{
			update_button_release_key(_key_left_shift);
		}
		if (GetAsyncKeyState(VK_RSHIFT) >= 0)
		{
			update_button_release_key(_key_right_shift);
		}
		break;
	case _key_control:
		if (GetAsyncKeyState(VK_LCONTROL) >= 0)
		{
			update_button_release_key(_key_left_control);
		}
		if (GetAsyncKeyState(VK_RCONTROL) >= 0)
		{
			update_button_release_key(_key_right_control);
		}
		break;
	case _key_menu:
		if (GetAsyncKeyState(VK_LMENU) >= 0)
		{
			update_button_release_key(_key_left_alt);
		}
		if (GetAsyncKeyState(VK_RMENU) >= 0)
		{
			update_button_release_key(_key_right_alt);
		}
		break;
	default:
		update_button_release_key(code);
		break;
	}
	return;
}

void input_windows_initialize_key_remapping(void)
{
	const HKL layout = GetKeyboardLayout(0);

	// Loop through every VK key and set the remapped key
	for (int32 vk_key = 0; vk_key < NUMBER_OF_KEYS; ++vk_key)
	{
		if (!ascii_to_key_table[vk_key].remapped)
		{
			const int16 remapped_key = (int16)MapVirtualKeyExA(vk_key, MAPVK_VK_TO_CHAR, layout);
			if (remapped_key)
			{
				ascii_to_key_table[vk_key].remapped = true;
				ascii_to_key_table[vk_key].remapped_character = remapped_key;
			}
		}
	}

	// Remap every key in the key remap array from the character to be printed
	// to the key that will print it
	for (int32 i = 0; i < REMAPPED_KEY_COUNT; ++i)
	{
		ASSERT(g_key_remap[i].key == i + FIRST_REMAPPED_KEY);

		const int16 result = VkKeyScanExW(g_key_remap[i].ch, layout);
		const bool shift_state_exists = (result & 0xFF00) != 0;

		// Make sure we only add a key remap if it doesn't have a shift state
		// Ex. pressing Shift/Ctrl and another character to print a specific character is not allowed
		if (!shift_state_exists)
		{
			g_key_remap[i].virtual_key = result;
			ascii_to_key_table[g_key_remap[i].virtual_key].key = g_key_remap[i].key;
		}
	}
	return;
}

uint8 __fastcall input_windows_get_remapped_key(e_input_key_code key_code)
{
	uint8 result = _key_not_a_key;
	if (key_code >= NUMBER_OF_KEYS)
	{
		if (key_code >= _keypad_unk_261)
		{
			ASSERT(VALID_INDEX(key_code - FIRST_REMAPPED_KEY, _countof(g_key_remap)));
			result = (uint8)g_key_remap[key_code - FIRST_REMAPPED_KEY].virtual_key;
		}
		else
		{
			switch (key_code)
			{
			case _keypad_left_control:
				result = _key_control;
				break;
			case _keypad_left_windows:
				result = _key_left_windows;
				break;
			case _keypad_left_menu:
				result = _key_menu;
				break;
			default:
				ASSERT(false);
			}
		}
	}
	else
	{
		result = (uint8)key_code;
	}
	return result;
}

void input_windows_clear_keyboard_input_state(void)
{
	g_keyboard_input_state.clear();
	return;
}

/* private code */

static void input_stop_removed_controller_handler_from_panicking(void)
{
	PatchCall(Memory::GetAddress(0x208D3C), input_has_gamepad_hook);
	PatchCall(Memory::GetAddress(0x2084B3), input_has_gamepad_hook);
	PatchCall(Memory::GetAddress(0x20844A), input_has_gamepad_hook);
	return;
}

static void input_rewrite_button_timing_loop(void)
{
	NopFill(Memory::GetAddressRelative(0x42FA8A), 3);	// Remove call to GetAsyncKeyState in input_update 
	NopFill(Memory::GetAddressRelative(0x42FAB9), 8);	// Remove loop code
	PatchCall(Memory::GetAddress(0x2FAAB), update_button_input_update_hook);
	return;
}

static e_input_key_code input_map_ascii_to_keycode(uint8 ascii)
{
	e_input_key_code result = (e_input_key_code)ascii;
	if (ascii_to_key_table[ascii].key != _key_not_a_key)
	{
		result = ascii_to_key_table[ascii].key;
	}
	return result;
}

static void __cdecl update_button(uint8* frames, uint16* msec, bool* key_bool, bool down, int32 elapsed_msec)
{
	if (H2Config_disable_ingame_keyboard)
	{
		down = false;
	}
	INVOKE(0x2E4C5, 0x0, update_button, frames, msec, key_bool, down, elapsed_msec);
	return;
}

static void __cdecl update_button_input_update_hook(uint8 * frames, uint16 * msec, bool* key_bool, bool down, int32 elapsed_msec)
{
	for (size_t code = 0; code < NUMBER_OF_KEYS; ++code)
	{
		update_button(
			&input_globals->keyboard.frames_down[code],
			&input_globals->keyboard.msec_down[code],
			&input_globals->keyboard.key_bool[code],
			g_keyboard_input_state.test(code),
			elapsed_msec
		);
	}
	return;
}

static void update_button_release_key(e_input_key_code code)
{
	update_button(
		&input_globals->keyboard.frames_down[code],
		&input_globals->keyboard.msec_down[code],
		&input_globals->keyboard.key_bool[code],
		false,
		0
	);
	g_keyboard_input_state.set(code, false);
	return;
}

static int compare_device_compatibility(const void* p1, const void* p2)
{
	input_device* device1 = *(input_device**)p1;
	input_device* device2 = *(input_device**)p2;

	XINPUT_STATE state_temp;
	uint32 error_code1, error_code2;
	error_code1 = device1->XGetState(&state_temp);
	error_code2 = device2->XGetState(&state_temp);


	if (error_code1 == ERROR_SEVERITY_SUCCESS && error_code2 == ERROR_DEVICE_NOT_CONNECTED)
		return -1;
	else if (error_code2 == ERROR_SEVERITY_SUCCESS && error_code1 == ERROR_DEVICE_NOT_CONNECTED)
		return 1;

	return 0;
}

static int compare_device_ports(const void* p1, const void* p2)
{
	xinput_device* device1 = *(xinput_device**)p1;
	xinput_device* device2 = *(xinput_device**)p2;

	return device1->get_port() - device2->get_port();
}

static void input_windows_update_device_mapping(void)
{
	input_device* g_new_xinput_order[k_number_of_controllers] = {};
	for (uint16 gamepad_index = _controller_index_0; gamepad_index < k_number_of_controllers; gamepad_index++)
	{
		g_new_xinput_order[gamepad_index] = g_xinput_devices[gamepad_index];
	}

	//sort based on connectivity, 
	//most active /first connected gamepads will be first in the array
	qsort(g_new_xinput_order, NUMBEROF(g_new_xinput_order), sizeof(input_device*), compare_device_compatibility);

	// we also rotate the controller array 
	// so that k_windows_device_controller_index does not have most active controller

	if (g_should_offset_gamepad_indices)
	{
		g_xinput_devices[_controller_index_0] = g_new_xinput_order[_controller_index_3];
		g_xinput_devices[_controller_index_1] = g_new_xinput_order[_controller_index_0];
		g_xinput_devices[_controller_index_2] = g_new_xinput_order[_controller_index_1];
		g_xinput_devices[_controller_index_3] = g_new_xinput_order[_controller_index_2];
	}
	return;
}

static void input_windows_restore_device_mapping(void)
{
	input_device* g_new_xinput_order[k_number_of_controllers] = {};
	for (uint16 gamepad_index = _controller_index_0; gamepad_index < k_number_of_controllers; gamepad_index++)
	{
		g_new_xinput_order[gamepad_index] = g_xinput_devices[gamepad_index];
	}

	//sort based on dwUserIndex (port)
	//this is set upon input_initialize()
	qsort(g_new_xinput_order, NUMBEROF(g_new_xinput_order), sizeof(input_device*), compare_device_ports);


	for (uint16 gamepad_index = _controller_index_0; gamepad_index < k_number_of_controllers; gamepad_index++)
	{
		g_xinput_devices[gamepad_index] = g_new_xinput_order[gamepad_index];
	}
}

static bool input_has_gamepad_hook(int16 gamepad_index, bool* a1)
{
	if (g_should_offset_gamepad_indices && gamepad_index == k_windows_device_controller_index)
	{
		if (a1)
		{
			*a1 = true;
		}
		return true;
	}

	return input_has_gamepad(gamepad_index, a1);
}
