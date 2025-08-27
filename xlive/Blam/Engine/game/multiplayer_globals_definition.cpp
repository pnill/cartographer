#include "stdafx.h"
#include "multiplayer_globals_definition.h"

#include "game_globals.h"
#include "cache/cache_files.h"

s_multiplayer_globals_definition* get_multiplayer_globals()
{
	s_game_globals* globals = scenario_get_game_globals();

	return (s_multiplayer_globals_definition*)tag_get_fast(globals->multiplayer_globals.index);
}