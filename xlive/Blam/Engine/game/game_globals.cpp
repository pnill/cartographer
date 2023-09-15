#include "stdafx.h"
#include "game_globals.h"

s_game_globals* scenario_get_game_globals(void)
{
	return *Memory::GetAddress<s_game_globals**>(0x479E70, 0x4A642C);
}

s_ui_levels_definition* game_globals_get_ui_levels(void)
{
	s_game_globals* globals = scenario_get_game_globals();
	if (globals->ui_level_data.size > 0 && globals->ui_level_data.data != NONE)
	{
		return globals->ui_level_data[0];
	}

	return NULL;
}
