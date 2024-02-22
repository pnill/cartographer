#include "stdafx.h"
#include "game_engine_simulation.h"

#include "game_engine.h"

void __cdecl game_engine_globals_set_player_entity_index(int16 player_index, int32 entity_index)
{
	game_engine_globals_get()->player_entity_index[player_index] = entity_index;
	return;
}