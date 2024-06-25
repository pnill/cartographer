#include "stdafx.h"
#include "rasterizer_lights.h"

/* public code */

bool rasterizer_rendering_light_get(void)
{
	return *Memory::GetAddress<bool*>(0xA3E3E7);
}
