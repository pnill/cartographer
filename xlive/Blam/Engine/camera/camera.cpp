#include "stdafx.h"
#include "camera.h"

#include "render/render_cameras.h"

/* public code */

render_camera* get_effect_camera(void)
{
	return Memory::GetAddress<render_camera*>(0xA3DF88);
}

void camera_apply_patches(void)
{
	return;
}
