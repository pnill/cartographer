#pragma once

/* constants */

enum
{
	k_maximum_number_of_active_screens = 36,
};

/* enums */

enum e_user_interface_render_window
{
	_first_render_window,
	_window_0 = _first_render_window,
	_window_1,
	_window_2,
	_window_3,
	_window_4,
	_last_render_window = _window_4,

	k_number_of_render_windows,
	k_no_window = NONE,
	k_any_window = 255
};

enum e_event_type
{
	_user_interface_event_type0 = 0,
	_user_interface_event_type_tab_up,
	_user_interface_event_type_tab_left,
	_user_interface_event_type_tab_down,
	_user_interface_event_type_tab_right,
	_user_interface_event_type_gamepad_button_pressed,
	_user_interface_event_type_mouse_button_left_click,
	_user_interface_event_type_keyboard_button_pressed,
	_user_interface_event_type8,
	_user_interface_event_type9,
	_user_interface_event_type_mouse_scroll_up,
	_user_interface_event_type_mouse_scroll_down,
	_user_interface_event_type_automation,
	_user_interface_event_type13,
	_user_interface_event_type14,
	_user_interface_event_type15,
};

// TODO : recheck these
enum e_user_interface_controller_component
{
	_controller_component_button_a = 0,
	_controller_component_button_b,
	_controller_component_button_x,
	_controller_component_button_y,
	_controller_component_button_black,
	_controller_component_button_white,
	_controller_component_button_left_trigger,
	_controller_component_button_right_trigger,
	_controller_component_button_dpad_up,
	_controller_component_button_dpad_down,
	_controller_component_button_dpad_left,
	_controller_component_button_dpad_right,
	_controller_component_button_start,
	_controller_component_button_back,
	_controller_component_button_left_thumb,
	_controller_component_button_right_thumb,
	_controller_component_any_stick,



	// keyboard_component are similar to Virtual KeyCodes (VK_KEY)
	_user_interface_keyboard_component_button_backspace = 0x8,
	_user_interface_keyboard_component_button_tab = 0x9,
	_user_interface_keyboard_component_button_enter = 0xD,
	_user_interface_keyboard_component_button_pause_break = 0x13,
	_user_interface_keyboard_component_button_capslock = 0x14,
	_user_interface_keyboard_component_button_escape = 0x1B,
	_user_interface_keyboard_component_button_space = 0x20,
	_user_interface_keyboard_component_button_page_up = 0x21,
	_user_interface_keyboard_component_button_page_down = 0x22,
	_user_interface_keyboard_component_button_end = 0x23,

	_user_interface_keyboard_component_button_left = 0x25,
	_user_interface_keyboard_component_button_up = 0x26,
	_user_interface_keyboard_component_button_right = 0x27,
	_user_interface_keyboard_component_button_down = 0x28,
	_user_interface_keyboard_component_button_insert = 0x2D,
	_user_interface_keyboard_component_button_delete = 0x2E,


	_user_interface_keyboard_component_button_numeric_zero = 0x30,
	_user_interface_keyboard_component_button_numeric_two = 0x32,
	// ....so on
	_user_interface_keyboard_component_button_numeric_nine = 0x39,


	_user_interface_keyboard_component_button_letter_a = 0x41,
	_user_interface_keyboard_component_button_letter_b = 0x42,
	// ....so on
	_user_interface_keyboard_component_button_letter_z = 0x5A,


	_user_interface_keyboard_component_button_f1 = 0x70,
	_user_interface_keyboard_component_button_f2 = 0x71,
	// ....so on
	_user_interface_keyboard_component_button_f12 = 0x7B,



	// ....special keys
	_user_interface_keyboard_component_button_numlock = 0x90,
	_user_interface_keyboard_component_button_left_shift = 0xA0,
	_user_interface_keyboard_component_button_right_shift = 0xA1,
	_user_interface_keyboard_component_button_left_control = 0xA2,
	_user_interface_keyboard_component_button_right_control = 0xA3,
	_user_interface_keyboard_component_button_left_alt = 0xA4,
	_user_interface_keyboard_component_button_right_alt = 0xA5,
	_user_interface_keyboard_component_button_tilde = 0x107,
	_user_interface_keyboard_component_button_minus = 0x108,
	_user_interface_keyboard_component_button_comma = 0x10E,
	_user_interface_keyboard_component_button_period = 0x10E,
	_user_interface_keyboard_component_button_backslash = 0x110,
	_user_interface_keyboard_component_button_equals = 0x112,
	_user_interface_keyboard_component_button_multiple = 0x115,
	_user_interface_keyboard_component_button_numeric_one = 0x117,
};

enum e_user_interface_channel_type
{
	_user_interface_channel_type_hardware_error = 0,
	_user_interface_channel_type_game_error,
	_user_interface_channel_type_virtual_keyboard,
	_user_interface_channel_type_gameshell_dialog,
	/*_user_interface_channel_type_online_menu,*/
	// got replaced with dialog_history_channel in h2v
	_user_interface_channel_type_gameshell_dialog_history,
	_user_interface_channel_type_gameshell_screen,
	_user_interface_channel_type_gameshell_background,
	k_number_of_user_interface_channels
};

/* typedefs */

typedef void* (__cdecl* proc_ui_screen_load_cb_t)(class c_screen_parameters*);

/* structures */

struct s_event_record
{
	e_event_type type;
	enum e_controller_index controller;
	e_user_interface_controller_component component;
	int16 event_value; // holds e_user_interface_automation_mode during _user_interface_event_type_automation
};
ASSERT_STRUCT_SIZE(s_event_record, 16);

struct s_screen_state
{
	int32 field_0;
	int32 m_last_focused_item_order;
	int32 m_last_focused_item_index;
};

class c_screen_parameters
{
public:
	c_screen_parameters(void);

	void initialize_default_user(
		uint16 user_flags,
		e_user_interface_channel_type channel_type,
		e_user_interface_render_window window_index,
		proc_ui_screen_load_cb_t load_cb);

	void initialize_internal(
		uint16 flags,
		uint16 user_flags,
		e_user_interface_channel_type channel_type,
		e_user_interface_render_window window_index,
		s_screen_state* screen_state,
		proc_ui_screen_load_cb_t load_cb);

	proc_ui_screen_load_cb_t get_laod_function(void)
	{
		return m_load_function;
	}

	void set_load_function(
		proc_ui_screen_load_cb_t function)
	{
		m_load_function = function;
		return;
	}

	void* execute_load_function(void)
	{
		return m_load_function(this);
	}

	int16 get_user_flags(
		void) const
	{
		return m_user_flags;
	}

	void set_user_flag(
		uint8 bit)
	{
		SET_BIT(m_user_flags, bit, true);
		return;
	}

	void set_user_flag_unsafe(
		int16 flags)
	{
		m_user_flags = flags;
		return;
	}

	e_user_interface_channel_type get_channel_type(
		void) const
	{
		return m_channel_type;
	}

	e_user_interface_render_window get_window_index(
		void) const
	{
		return m_window_index;
	}

private:
	uint16 m_flags;
	int16 m_user_flags;
	e_user_interface_channel_type m_channel_type;
	e_user_interface_render_window m_window_index;
	void* m_context;
	s_screen_state m_screen_state;
	proc_ui_screen_load_cb_t m_load_function;
};
ASSERT_STRUCT_SIZE(c_screen_parameters, 0x20);

/* prototypes */

bool __cdecl user_interface_automation_is_active(void);
uint32 __cdecl user_interface_milliseconds(void);

class c_user_interface_channel* __fastcall user_interface_get_channel(e_user_interface_render_window window_index, e_user_interface_channel_type channel_index);

bool __cdecl user_interface_error_display_allowed(void);
bool __cdecl user_interface_has_responding_controller(int32 user_index);
bool __cdecl user_interface_channel_is_busy(e_user_interface_channel_type channel_type);
bool __cdecl user_interface_back_out_from_channel_by_id(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, enum e_user_interface_screen_id id);

int32 __cdecl user_interface_get_screen_tag_index_by_id(enum e_user_interface_screen_id id);

bool __cdecl user_interface_in_screen(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, enum e_user_interface_screen_id screen_id);
bool __cdecl user_interface_error_screen_is_active(e_user_interface_channel_type channel_index, e_user_interface_render_window window_index, enum e_ui_error_types error_code);

void __cdecl screen_error_ok_dialog_show(e_user_interface_channel_type channel_type, enum e_ui_error_types ui_error_index, e_user_interface_render_window window_index, int16 user_flags, void* ok_callback, void* fallback);
void __cdecl screen_error_ok_dialog_with_custom_text(e_user_interface_channel_type channel_type, enum e_ui_error_types ui_error_index, e_user_interface_render_window window_index, int16 user_flags, void* ok_callback, void* fallback, const wchar_t* custom_title, const wchar_t* custom_body);

void __cdecl user_interface_error_ok_cancel_dialog_show_confirmation(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags, void* ok_callback_handle, enum e_ui_error_types error_type);
void __cdecl user_interface_back_out_from_channel(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index);
void __cdecl user_interface_enter_game_shell(int32 context);

void __cdecl render_menu_user_interface(int32 controller_index, e_user_interface_render_window render_window, rectangle2d* out_rect2d);

void __cdecl user_interface_return_to_mainmenu(bool a1);

void __cdecl user_interface_update(real32 dt);

uint32 user_interface_set_context_presence(uint32 game_mode);

#ifdef UI_DEBUG
void user_interface_debug_load_main_menu(void);

void user_interface_debug_text_bounds(bool value);

void debug_render_title_safe_bounds(bool value);

void set_debug_frame_element_bounds(bool value);

void user_interface_test_screen(const char* screen);

void user_interface_test_transition_out_console_screen(void);

void set_debug_show_screen_tag_path(bool value);

void user_interface_show_current_screen_tag(const char* path);

void debug_set_ui_beta(bool value);

void user_interface_test_error_ok(int16 id);

void user_interface_test_error_ok_cancel(int16 id);

void user_interface_test_confirmation(int16 id);
#endif

bool __cdecl user_interface_globals_is_beta_build(void);
int32 __cdecl user_interface_globals_get_game_difficulty(void);
int32 __cdecl user_interface_globals_get_edit_player_profile_index(void);
struct s_saved_game_player_profile* user_interface_globals_get_edit_player_profile(void);
enum e_scenario_type __cdecl user_interface_globals_get_map_type(void);
void __cdecl user_interface_globals_set_game_difficulty_real(int32 difficulty);
void __cdecl user_interface_globals_set_loading_from_persistent_storage(bool a1);
void __cdecl user_interface_globals_commit_edit_profile_changes(void);
void __cdecl user_interface_globals_save_profile_changes_to_disk(void);
void __cdecl user_interface_globals_finish_saving_profile_changes(void);
void __cdecl user_interface_globals_set_edit_player_profile(enum e_controller_index controller_index, uint32 profile_index, struct s_saved_game_player_profile* profile);
