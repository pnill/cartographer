#pragma once
#include "Blam/Cache/DataTypes/DatumIndex.h"
#include "Blam/Engine/Objects/ObjectPlacementData.h"

namespace Engine
{
	game_life_cycle get_game_life_cycle();
	bool __cdecl IsGameMinimized();
	e_engine_type get_current_engine_type();
	int get_global_random_seed_address();

	namespace Unit
	{
		int __cdecl remove_equipment(datum unit_datum_index);
		signed int __cdecl inventory_next_weapon(unsigned short unit);
		bool __cdecl assign_equipment_to_unit(datum unit, int object_index, short unk);
	}

	namespace Objects
	{
		char* __cdecl try_and_get_data_with_type(datum object_datum_index, int object_type_flags);
		void __cdecl create_new_placement_data(ObjectPlacementData* s_object_placement_data, datum object_definition_index, datum object_owner, int unk);
		int __cdecl call_object_new(ObjectPlacementData* pObject);
	}
}