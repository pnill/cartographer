#pragma once

#include "Blam\Cache\DataTypes\BlamDataTypes.h"

namespace ObjectInterpolate
{
	void PreGameTickUpdate();
	void PostGameTickUpdate();
	void Reset();
	void ResetObject(datum object_idx);
	void ApplyPatches();
	int GetObjectInterpolateCount();
}
