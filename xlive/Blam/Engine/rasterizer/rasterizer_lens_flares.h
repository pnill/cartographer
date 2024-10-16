#pragma once
#include "rasterizer/dx9/rasterizer_dx9_targets.h"

/* public code */

void rasterizer_lens_flares_apply_patches(void);

e_rasterizer_target __cdecl rasterizer_suns_glow_occlude(e_rasterizer_target rasterizer_target);

e_rasterizer_target rasterizer_suns_glow_draw(e_rasterizer_target rasterizer_target);

e_rasterizer_target __cdecl lens_flares_draw(e_rasterizer_target rasterizer_target);
