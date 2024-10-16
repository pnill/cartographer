#include "stdafx.h"
#include "render_water.h"

/* public code */

bool __cdecl render_water(bool water_enabled, bool clear_target)
{
	return INVOKE(0x1A0673, 0x0, render_water, water_enabled, clear_target);
}
