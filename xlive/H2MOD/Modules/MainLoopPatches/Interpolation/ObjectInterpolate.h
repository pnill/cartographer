#pragma once

#include "Blam\Cache\DataTypes\BlamDataTypes.h"

namespace ObjectInterpolate
{
	void PreGameTickLocalPlayersUpdate();
	void PostGameTickLocalPlayersUpdate();
	void Reset();
	void ResetObject(datum object_idx);
	void ApplyPatches();
	int GetObjectInterpolateCount();
}
