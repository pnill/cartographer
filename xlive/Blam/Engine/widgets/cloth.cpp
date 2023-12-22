#include "stdafx.h"
#include "cloth.h"

#include "Blam/Engine/hs/hs.h"
#include "Util/Hooks/Hook.h"

void apply_cloth_patches()
{
	PatchCall(Memory::GetAddress(0x18E4DD, 0), hs_object_get_markers_by_string_id);
}
