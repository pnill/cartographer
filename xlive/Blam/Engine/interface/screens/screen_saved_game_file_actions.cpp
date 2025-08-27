#include "stdafx.h"
#include "screen_saved_game_file_actions.h"

void* __cdecl c_screen_saved_game_file_actions::load_settings(s_screen_parameters* parameters)
{
	return INVOKE(0x24C469, 0, load_settings, parameters);
}
