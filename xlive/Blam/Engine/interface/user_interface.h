#pragma once
#include "user_interface_shared_globals.h"
#include "saved_games/game_variant.h"
#include "saved_games/saved_game_files.h"

/* enums */

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

enum e_user_interface_render_window
{
	_window_0,
	_window_1,
	_window_2,
	_window_3,
	_window_4,

	k_number_of_render_windows = 5
};

/* typedefs */

typedef void* (__cdecl* proc_ui_screen_load_cb_t)(struct s_screen_parameters*);

/* structures */

struct s_screen_state
{
	int32 field_0;
	int32 m_last_focused_item_order;
	int32 m_last_focused_item_index;
};

struct s_screen_parameters
{
	uint16 m_flags;
	int16 m_user_flags;
	e_user_interface_channel_type m_channel_type;
	e_user_interface_render_window m_window_index;
	void* m_context;
	s_screen_state m_screen_state;
	proc_ui_screen_load_cb_t m_load_function;

	void data_new(uint16 flags, uint16 user_flags, e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, proc_ui_screen_load_cb_t load_cb)
	{
		this->m_flags = flags;
		this->m_user_flags = user_flags;
		this->m_channel_type = channel_type;
		this->m_window_index = window_index;
		m_screen_state.field_0 = NONE;
		m_screen_state.m_last_focused_item_order = NONE;
		m_screen_state.m_last_focused_item_index = NONE;
		this->m_load_function = load_cb;
	}

	void* ui_screen_load_proc_exec()
	{
		return m_load_function(this);
	}
};
ASSERT_STRUCT_SIZE(s_screen_parameters, 0x20);

/* forward declations*/
enum e_user_interface_screen_id : uint32;

/* prototypes */

void user_interface_apply_patches(void);

bool __cdecl user_interface_automation_is_active(void);
uint32 __cdecl user_interface_milliseconds(void);
bool __cdecl user_interface_error_display_allowed(void);
bool __cdecl user_interface_has_responding_controller(int32 user_index);
bool __cdecl user_interface_channel_is_busy(e_user_interface_channel_type channel_type);
bool __cdecl user_interface_back_out_from_channel_by_id(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, e_user_interface_screen_id id);
bool __cdecl user_interface_in_screen(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, e_user_interface_screen_id screen_id);
bool __cdecl user_interface_error_screen_is_active(e_user_interface_channel_type channel_index, e_user_interface_render_window window_index, e_ui_error_types error_code);

void __cdecl screen_error_ok_dialog_show(e_user_interface_channel_type channel_type, e_ui_error_types ui_error_index, e_user_interface_render_window window_index, int16 user_flags, void* ok_callback, void* fallback);
void __cdecl screen_error_ok_dialog_with_custom_text(e_user_interface_channel_type channel_type, e_ui_error_types ui_error_index, e_user_interface_render_window window_index, int16 user_flags, void* ok_callback, void* fallback, const wchar_t* custom_title, const wchar_t* custom_body);

void __cdecl user_interface_error_ok_cancel_dialog_show_confirmation(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags, void* ok_callback_handle, e_ui_error_types error_type);
void __cdecl user_interface_back_out_from_channel(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index);
void __cdecl user_interface_enter_game_shell(int32 context);

void __cdecl user_interface_set_variant(enumerated_file_index enumerated_file_index, s_game_variant* variant);

s_game_variant* __cdecl user_interface_get_variant();

void __cdecl user_interface_clear_variant();

bool __cdecl user_interface_construct_default_game_variant_from_file_type(s_game_variant* out_variant, e_saved_game_file_type type);

void __cdecl render_menu_user_interface(int32 controller_index, e_user_interface_render_window render_window, rectangle2d* out_rect2d);

void __cdecl user_interface_return_to_mainmenu(bool a1);

void __cdecl user_interface_update(real32 dt);

uint32 user_interface_set_context_presence(e_context_presence game_mode);

void user_interface_debug_load_main_menu();
void user_interface_debug_text_bounds(bool value);
void user_interface_debug_show_title_safe_bounds(bool value);
void user_interface_debug_element_bounds(bool value);
void user_interface_transition_out_console_window();
void user_interface_set_beta(bool value);
void user_interface_test_error_ok(int16 id);
void user_interface_test_error_ok_cancel(int16 id);
void user_interface_test_confirmation(int16 id);