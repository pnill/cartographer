#pragma once

#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Engine/game/GameOptions.h"
#include "Blam/Engine/objects/object_placement.h"

namespace Engine
{
	e_game_life_cycle get_game_life_cycle();
	e_engine_type get_current_engine_type();
	int get_global_random_seed_address();
	int get_game_mode_engine();

	namespace Unit
	{
		void __cdecl remove_equipment(datum unit_idx);
		signed int __cdecl inventory_next_weapon(datum unit_idx);
		bool __cdecl assign_equipment_to_unit(datum unit_idx, datum object_idx, short a3);
	}
}