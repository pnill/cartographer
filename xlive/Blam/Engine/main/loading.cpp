#include "stdafx.h"
#include "loading.h"

#include "Util/Hooks/Hook.h"

void loading_apply_patches(void)
{
	// Removes call to main_loading_idle to improve framerates
	// Used for playing while disc installs (not supported in carto)
	NopFill(Memory::GetAddress(0x39BAB), 5);
	return;
}
