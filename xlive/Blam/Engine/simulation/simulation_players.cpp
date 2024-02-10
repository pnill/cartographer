#include "stdafx.h"
#include "simulation_players.h"

bool __cdecl simulation_players_apply_update(simulation_player_update* player_update)
{
	return INVOKE(0x1E22E2, 0x1C930E, simulation_players_apply_update, player_update);
}
