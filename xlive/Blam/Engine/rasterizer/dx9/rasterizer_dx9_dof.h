#pragma once

/* public code */

void __cdecl rasterizer_dx9_dof_render_bloom(real32 blur_amount, real32 threshold, real32 brightness, real32 box_factor, real32 max_factor, bool a6, bool a7);

void __cdecl rasterizer_dx9_render_depth_of_field(real32 lower, real32 upper, real32 a3);
