#pragma once
#include "saved_games/saved_game_files.h"

enum e_user_interface_channel_type;
enum e_user_interface_render_window;
enum e_ui_error_types : uint32;
struct s_screen_parameters;

class c_screen_error_dialog_ok
{
public:
	static void* __cdecl load_for_active_users(s_screen_parameters* parameters);
	static void load_for_disk_result(int16 controllers_mask, e_saved_game_disk_result disk_result);
	static void apply_patches();
};


class c_screen_error_dialog_ok_cancel
{
public:
	static void show_dialog(e_user_interface_channel_type channel_type, e_ui_error_types error_type, e_user_interface_render_window window_index, uint16 user_flags, void* ok_callback, void* fallback, int a7, int a8);
	static void apply_patches();
};