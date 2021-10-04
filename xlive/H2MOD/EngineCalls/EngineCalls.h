#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Engine/Game/GameEngine.h"
#include "Blam/Engine/Objects/ObjectPlacementData.h"

namespace EngineCalls
{
	game_life_cycle get_game_life_cycle();
	bool __cdecl IsGameMinimized();
	e_engine_type get_current_engine_type();
	int get_global_random_seed_address();
	bool game_is_campaign();
	bool game_is_multiplayer();
	int get_game_mode_engine();
	s_game_variant* get_game_variant();
	namespace Unit
	{
		int __cdecl remove_equipment(datum unit_datum_index);
		signed int __cdecl inventory_next_weapon(unsigned short unit);
		bool __cdecl assign_equipment_to_unit(datum unit, int object_index, short unk);
	}

	namespace Objects
	{
		void __cdecl create_new_placement_data(s_object_placement_data* s_object_placement_data, datum object_definition_index, datum object_owner, int unk);
		int __cdecl call_object_new(s_object_placement_data* pObject);
		void apply_biped_object_definition_patches();
		bool __cdecl call_add_object_to_sync(datum gamestate_object_datum);
		void __cdecl object_destroy(datum gamestate_object_datum);
	}
}