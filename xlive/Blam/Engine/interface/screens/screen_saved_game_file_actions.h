#pragma once
#include "interface/user_interface_widget_window.h"


class c_screen_saved_game_file_actions : public c_screen_with_menu
{
public:
	static void* __cdecl load_settings(s_screen_parameters* parameters);
};
