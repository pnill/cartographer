#include "stdafx.h"
#include "render_lights.h"

/* public code */

void __cdecl render_lights(void)
{
	INVOKE(0x14CB17, 0x0, render_lights);
	return;
}

void __cdecl render_light_clear_data(void)
{
	INVOKE(0x1A0F39, 0x0, render_light_clear_data);
	return;
}

void __cdecl render_cinematic_lightmap_shadows(int32 effect_flag)
{
	INVOKE(0x193F9D, 0x0, render_cinematic_lightmap_shadows, effect_flag);
	return;
}

void __cdecl render_lights_new(void)
{
	INVOKE(0x193072, 0x0, render_lights_new);
	return;
}
