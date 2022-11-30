#include "stdafx.h"
#include "game_engine_king.h"

#include "H2MOD/Modules/CustomVariantSettings/CustomVariantSettings.h"
#include "Util/Hooks/Hook.h"

namespace game_engine_king
{
	typedef int(__cdecl get_next_hill_index_t)(int previousHill);
	get_next_hill_index_t* p_get_next_hill_index;
	signed int __cdecl carto_get_next_hill_index(int previousHill)
	{
		static int currentPredefinedIndex = 0;
		int hillCount = *Memory::GetAddress<int*>(0x4dd0a8, 0x5008e8);

		//Return -1 to tell the engine there is no koth hills on the map.
		if (hillCount <= 0)
			return -1;
		switch (currentVariantSettings.hillRotation)
		{
		case CustomVariantSettings::_sequential:
			if (previousHill + 1 >= hillCount)
				return 0;
			return previousHill + 1;
		case CustomVariantSettings::_reverse:
			if (previousHill - 1 <= 0)
				return hillCount;
			return previousHill - 1;
		case CustomVariantSettings::_predefined:
			if (currentPredefinedIndex == 15)
				currentPredefinedIndex = 0;
			else if (currentVariantSettings.predefinedHillSet[currentPredefinedIndex + 1] == 0)
				currentPredefinedIndex = 0;
			else
				++currentPredefinedIndex;
			return currentVariantSettings.predefinedHillSet[currentPredefinedIndex] - 1;
		// If no custom variant type specified for KOTH revert to default logic
		default:
			return p_get_next_hill_index(previousHill);
		}
	}

	void ApplyPatches()
	{
		p_get_next_hill_index = Memory::GetAddress<get_next_hill_index_t*>(0x10DF1E, 0xDA4CE);
		PatchCall(Memory::GetAddress(0x10FE1F, 0xDC3CF), carto_get_next_hill_index);
		PatchCall(Memory::GetAddress(0x10FE55, 0xDC405), carto_get_next_hill_index);
	}
}