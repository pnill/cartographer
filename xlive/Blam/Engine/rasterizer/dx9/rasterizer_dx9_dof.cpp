#include "stdafx.h"
#include "rasterizer_dx9_dof.h"

/* public code */

void __cdecl rasterizer_dx9_dof_render_bloom(real32 blur_amount, real32 threshold, real32 brightness, real32 box_factor, real32 max_factor, bool a6, bool a7)
{
	INVOKE(0x26C3EB, 0x0, rasterizer_dx9_dof_render_bloom, blur_amount, threshold, brightness, box_factor, max_factor, a6, a7);
	return;
}

void __cdecl rasterizer_dx9_render_depth_of_field(real32 lower, real32 upper, real32 a3)
{
	INVOKE(0x26CABC, 0x0, rasterizer_dx9_render_depth_of_field, lower, upper, a3);
	return;
}
