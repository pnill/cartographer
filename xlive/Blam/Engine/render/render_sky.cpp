#include "stdafx.h"
#include "render_sky.h"

/* public code */

real32 __cdecl get_current_sky_render_model_scale(void)
{
	return INVOKE(0x19A139, 0x186E29, get_current_sky_render_model_scale);
}

void __cdecl render_sky_model(void)
{
	INVOKE(0x19A099, 0x0, render_sky_model);
	return;
}

void __cdecl render_sky(void)
{
	INVOKE(0x19A377, 0x0, render_sky);
	return;
}

void __cdecl render_sky_lens_flares(void)
{
	INVOKE(0x19A43A, 0x0, render_sky_lens_flares);
	return;
}
