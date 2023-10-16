#pragma once
#include "simulation_game_objects.h"

struct s_simulation_turret_creation_data
{
	s_simulation_object_creation_data object;
	datum simulation_entity_index;
};
CHECK_STRUCT_SIZE(s_simulation_projectile_creation_data, 20);
