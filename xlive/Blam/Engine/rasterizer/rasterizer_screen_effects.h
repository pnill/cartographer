#pragma once
#include "rasterizer/dx9/rasterizer_dx9_targets.h"

/* public code */

e_rasterizer_target __cdecl rasterizer_screen_effects_render(
    datum screen_effect_tag_index,
    real32* interpolator_results,
    int32 input_count,
    int32 input_count_1,
    real32* mesh_data);
