#include "stdafx.h"

#include "Engine.h"

namespace Engine
{
	namespace Unit
	{
		void __cdecl remove_equipment(datum unit_idx)
		{
			//LOG_TRACE_GAME("unit_reset_equipment(unit_datum_index: %08X)", unit_datum_index);
			typedef void(__cdecl* unit_reset_equipment_t)(datum unit_idx);
			auto p_unit_reset_equipment = Memory::GetAddress<unit_reset_equipment_t>(0x1441E0, 0x133030);
			if (!DATUM_IS_NONE(unit_idx))
			{
				p_unit_reset_equipment(unit_idx);
			}
		}

		int __cdecl inventory_next_weapon(datum unit_idx)
		{
			//LOG_TRACE_GAME("unit_inventory_next_weapon(unit: %08X)", unit);
			typedef int(__cdecl* unit_inventory_next_weapon_t)(datum);
			auto p_unit_inventory_next_weapon = Memory::GetAddress<unit_inventory_next_weapon_t>(0x139E04, 0x0);

			return p_unit_inventory_next_weapon(unit_idx);
		}

		bool __cdecl assign_equipment_to_unit(datum unit_idx, datum object_idx, short a3)
		{
			//LOG_TRACE_GAME("assign_equipment_to_unit(unit: %08X,object_index: %08X,unk: %08X)", unit, object_index, unk);
			typedef bool(__cdecl* assign_equipment_to_unit_t)(datum unit_idx, datum object_idx, short a3);
			auto passign_equipment_to_unit = Memory::GetAddress<assign_equipment_to_unit_t>(0x1442AA, 0x1330FA);

			return passign_equipment_to_unit(unit_idx, object_idx, a3);
		}
	}
}