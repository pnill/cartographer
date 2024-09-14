#pragma once
#include "effects/decals.h"

/* typedefs */

typedef void(__cdecl* draw_proc_t)(e_decal_layer, int16);

/* prototypes */

void __cdecl render_decal_visibility(draw_proc_t draw_proc, int32 a2);
