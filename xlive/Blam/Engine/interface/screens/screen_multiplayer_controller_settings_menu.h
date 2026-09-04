#pragma once
#include "interface/user_interface_widget_window.h"

// TODO: finish reversing
class c_screen_multiplayer_controller_settings : public c_screen_with_menu
{
public:

	static void* load(
		class c_screen_parameters* parameters)
	{
		return INVOKE(0x24E121, 0, c_screen_multiplayer_controller_settings::load, parameters);
	}
};
