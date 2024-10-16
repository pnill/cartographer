#include "stdafx.h"
#include "liquid.h"

#include "objects/objects.h"

/* public code */

void liquid_apply_patches(void)
{
	// replace marker calls to interpolated one
	//PatchCall(Memory::GetAddress(0x18658B), object_get_markers_by_string_id);
	return;
}

void __cdecl liquid_render(void)
{
	INVOKE(0x186628, 0x0, liquid_render);
	return;
}
