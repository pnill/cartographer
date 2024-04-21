#include "stdafx.h"
#include "render_sky.h"

real32 __cdecl get_current_sky_render_model_scale(void)
{
	return INVOKE(0x19A139, 0x186E29, get_current_sky_render_model_scale);
}

void __cdecl render_sky_model(void)
{
	INVOKE(0x19A099, 0x0, get_current_sky_render_model_scale);
	return;
}
