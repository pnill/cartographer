#include "stdafx.h"
#include "camera.h"

#include "objects/objects.h"

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
}
