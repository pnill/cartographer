#pragma once

#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Engine\Objects\ObjectPlacementData.h"

namespace Engine
{
	e_game_life_cycle get_game_life_cycle();

	namespace Unit
	{
		void __cdecl remove_equipment(datum unit_idx);
		signed int __cdecl inventory_next_weapon(datum unit_idx);
		bool __cdecl assign_equipment_to_unit(datum unit_idx, datum object_idx, short a3);
	}
}