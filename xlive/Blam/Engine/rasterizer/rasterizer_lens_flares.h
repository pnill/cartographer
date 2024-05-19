#pragma once
#include "rasterizer/dx9/rasterizer_dx9_targets.h"

/* public code */

void lens_flare_fix(void);

e_rasterizer_target __cdecl rasterizer_suns_glow_draw(e_rasterizer_target target);
