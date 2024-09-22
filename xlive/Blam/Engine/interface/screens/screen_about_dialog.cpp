#include "stdafx.h"
#include "screen_about_dialog.h"

void* c_screen_about_dialog_menu::load(s_screen_parameters* parameters)
{
	return INVOKE(0x21F681, 0x0, c_screen_about_dialog_menu::load, parameters);
}
