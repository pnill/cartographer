#include "stdafx.h"
#include "camera.h"

#include "Blam/Engine/objects/objects.h"
#include "Util/Hooks/Hook.h"

s_camera* get_global_camera()
{
	return Memory::GetAddress<s_camera*>(0x4E66C8);
}

s_camera* get_effect_camera()
{
	return Memory::GetAddress<s_camera*>(0xA3DF88);
}

void camera_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x13D3CF), object_get_markers_by_string_id);
	PatchCall(Memory::GetAddress(0x13D48D), object_get_markers_by_string_id);
}
