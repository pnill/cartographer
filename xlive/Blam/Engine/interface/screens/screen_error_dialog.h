#pragma once

/* classes */

class c_screen_error_dialog_ok
{
public:
	static void* show_dialog(
		enum e_user_interface_channel_type channel_type,
		enum e_ui_error_types error_type,
		enum e_user_interface_render_window window_index,
		uint16 user_flags,
		void* ok_callback,
		void* fallback
	);
	static void* __cdecl load_for_active_users(class c_screen_parameters* parameters);
	static void apply_patches();
};


class c_screen_error_dialog_ok_cancel
{
public:
	static void* show_dialog(
		enum e_user_interface_channel_type channel_type,
		enum e_ui_error_types error_type,
		enum e_user_interface_render_window window_index,
		uint16 user_flags,
		void* ok_callback,
		void* fallback,
		int32 a7,
		int32 a8);
	static void apply_patches();
};
