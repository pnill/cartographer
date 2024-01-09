#pragma once
#include "simulation_game_objects.h"
#include "Blam/Engine/objects/damage.h"

struct s_simulation_projectile_creation_data
{
	s_simulation_object_creation_data object;
	s_damage_owner damage;
	bool tracer;
	bool disable_deceleration;
	int16 pad1;
};
CHECK_STRUCT_SIZE(s_simulation_projectile_creation_data, 32);
