#include "stdafx.h"
#include "rasterizer_screen_effects.h"

#include "rasterizer/dx9/rasterizer_dx9_targets.h"

/* public code */

e_rasterizer_target __cdecl rasterizer_screen_effects_render(
    datum screen_effect_tag_index,
    real32* interpolator_results,
    int32 input_count,
    int32 input_count_1,
    real32* mesh_data)
{
    return INVOKE(0x283DFD, 0x0, rasterizer_screen_effects_render,
        screen_effect_tag_index, 
        interpolator_results, 
        input_count,
        input_count_1,
        mesh_data);
}
