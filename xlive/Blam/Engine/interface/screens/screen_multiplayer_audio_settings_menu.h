#pragma once
#include "interface/user_interface_widget_window.h"

// TODO: finish reversing
class c_screen_multiplayer_audio_settings : public c_screen_with_menu
{
public:

	static void* load(
		class c_screen_parameters* parameters)
	{
		return INVOKE(0x24DD80, 0, c_screen_multiplayer_audio_settings::load, parameters);
	}
};
