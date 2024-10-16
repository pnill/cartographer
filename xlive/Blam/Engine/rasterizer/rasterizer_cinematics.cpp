#include "stdafx.h"
#include "rasterizer_cinematics.h"

/* public code */

void __cdecl rasterizer_cinematics_initialize(void)
{
	INVOKE(0x2705F4, 0x0, rasterizer_cinematics_initialize);
	return;
}

s_rasterizer_cinematics_globals* __cdecl rasterizer_cinematic_screen_effect_update(s_rasterizer_cinematics_globals* globals)
{
	return INVOKE(0x270713, 0x0, rasterizer_cinematic_screen_effect_update, globals);
}
