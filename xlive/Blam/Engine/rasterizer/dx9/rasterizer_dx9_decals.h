#pragma once
#include "effects/decals.h"

/* prototypes */

void __cdecl rasterizer_dx9_decals_begin(e_decal_layer layer);

void __cdecl rasterizer_dx9_decals_end(void);

void __cdecl rasterizer_dx9_decals_draw(e_decal_layer layer, int16 a2);
