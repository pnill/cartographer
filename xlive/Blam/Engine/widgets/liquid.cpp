#include "stdafx.h"
#include "liquid.h"

#include "Blam/Engine/objects/objects.h"
#include "Util/Hooks/Hook.h"

void liquid_apply_patches(void)
{
	// replace marker calls to interpolated one
	//PatchCall(Memory::GetAddress(0x18658B), object_get_markers_by_string_id);
	return;
}