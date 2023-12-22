#include "stdafx.h"
#include "loading.h"

#include "Util/Hooks/Hook.h"

void loading_apply_patches(void)
{
	NopFill(Memory::GetAddress(0x39BAB), 5);
	return;
}
