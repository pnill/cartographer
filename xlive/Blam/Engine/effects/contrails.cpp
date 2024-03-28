#include "stdafx.h"
#include "contrails.h"

#include "objects/objects.h"


void contrails_apply_patches(void)
{
	// replace marker call to interpolated ones
	PatchCall(Memory::GetAddress(0xAB873), object_get_markers_by_string_id);
	return;
}