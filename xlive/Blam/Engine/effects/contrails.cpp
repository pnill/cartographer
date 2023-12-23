#include "stdafx.h"
#include "contrails.h"

#include "Blam/Engine/objects/objects.h"
#include "Util/Hooks/Hook.h"

void contrails_apply_patches(void)
{
	// replace marker call to interpolated ones
	PatchCall(Memory::GetAddress(0xAB873), object_get_markers_by_string_id);
	return;
}