#include "stdafx.h"

/* public code */

bool __cdecl rasterizer_transparent_geometry_initialize(void)
{
	return INVOKE(0x278271, 0x0, rasterizer_transparent_geometry_initialize);
}

void __cdecl rasterizer_transparent_geometry_reset_counts(void)
{
	INVOKE(0x27818C, 0x0, rasterizer_transparent_geometry_reset_counts);
	return;
}

void __cdecl transparent_geometry_draw(void)
{
	INVOKE(0x2787F3, 0x0, transparent_geometry_draw);
	return;
}
