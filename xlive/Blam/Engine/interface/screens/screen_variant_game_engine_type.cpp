#include "stdafx.h"
#include "screen_variant_game_engine_type.h"

void* c_screen_game_engine_category::load_for_settings(s_screen_parameters* parameters)
{
	return INVOKE(0x21F16E, 0x0, c_screen_game_engine_category::load_for_settings, parameters);
}
