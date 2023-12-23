#include "stdafx.h"
#include "cloth.h"

#include "Blam/Engine/hs/hs.h"
#include "Util/Hooks/Hook.h"

void apply_cloth_patches(void)
{
	// disable cloth debugging that writes to cloth.txt
	WriteValue<bool>(Memory::GetAddress(0x41F650), false);
	return;
}
