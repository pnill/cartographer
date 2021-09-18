#include "Engine.h"

namespace Engine
{
	namespace Unit
	{
		int __cdecl remove_equipment(datum unit_datum_index)
		{
			//LOG_TRACE_GAME("unit_reset_equipment(unit_datum_index: %08X)", unit_datum_index);
			typedef int(__cdecl unit_reset_equipment)(datum unit_datum_index);
			auto p_unit_reset_equipment = Memory::GetAddress<unit_reset_equipment*>(0x1441E0, 0x133030);
			if (!DATUM_IS_NONE(unit_datum_index))
			{
				return p_unit_reset_equipment(unit_datum_index);
			}
			return 0;
		}

		signed int __cdecl inventory_next_weapon(unsigned short unit)
		{
			//LOG_TRACE_GAME("unit_inventory_next_weapon(unit: %08X)", unit);
			typedef signed int(__cdecl unit_inventory_next_weapon)(unsigned short unit);
			auto p_unit_inventory_next_weapon = Memory::GetAddress<unit_inventory_next_weapon*>(0x139E04);

			return p_unit_inventory_next_weapon(unit);
		}

		bool __cdecl assign_equipment_to_unit(datum unit, int object_index, short unk)
		{
			//LOG_TRACE_GAME("assign_equipment_to_unit(unit: %08X,object_index: %08X,unk: %08X)", unit, object_index, unk);
			typedef bool(__cdecl assign_equipment_to_unit)(datum unit, int object_index, short unk);
			auto passign_equipment_to_unit = Memory::GetAddress<assign_equipment_to_unit*>(0x1442AA, 0x1330FA);

			return passign_equipment_to_unit(unit, object_index, unk);
		}
	}
}