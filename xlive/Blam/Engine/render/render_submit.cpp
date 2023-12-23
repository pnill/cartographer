#include "stdafx.h"
#include "render_submit.h"

#include "Blam/Engine/objects/objects.h"
#include "Util/Hooks/Hook.h"

void render_submit_apply_patches(void)
{
	// replace marker call to interpolated ones
	PatchCall(Memory::GetAddress(0x19BAFC), object_get_markers_by_string_id);
	return;
}
