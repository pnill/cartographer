#pragma once
#include "rasterizer_dx9_targets.h"

/* public code */

void __cdecl rasterizer_dx9_dof_render_bloom(real32 blur_amount, real32 threshold, real32 brightness, real32 box_factor, real32 max_factor, bool a6, bool a7);

void __cdecl rasterizer_dx9_render_depth_of_field(real32 lower, real32 upper, real32 a3);

e_rasterizer_target rasterizer_dx9_convolve_screen_surfaces(
	real32 a1,
	real32 a2,
	real32 a3,
	e_rasterizer_target surface_source,
	e_rasterizer_target surface_destination,
	e_rasterizer_target target_3,
	e_rasterizer_target stretch_destination,
	int32 a8,
	int32 a9,
	real32 a10,
	real32 a11);

void rasterizer_dx9_draw_overlay_rect(uint32 screen_resolution_x, uint32 screen_resolution_y, D3DCOLOR color);
