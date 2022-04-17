#pragma once

#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Engine\Game\GameOptions.h"
#include "Blam\Engine\Objects\ObjectPlacementData.h"

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

	namespace Objects
	{
		void create_new_placement_data(s_object_placement_data* object_placement_data, datum object_definition_idx, datum object_owner_idx, int a4);
		datum object_new(s_object_placement_data* object_placement_data);
		void apply_biped_object_definition_patches();
		void simulation_action_object_create(datum object_idx);
		void object_destroy(datum object_idx);
	}
}