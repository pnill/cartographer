#pragma once
#include "game/players.h"

#define k_maximum_multiplayer_players k_maximum_players

struct s_simulation_game_engine_player_creation_data
{
	uint16 absolute_player_index;
};
CHECK_STRUCT_SIZE(s_simulation_game_engine_player_creation_data, 2);
