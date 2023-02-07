#pragma once
#include "Blam/Math/real_math.h"
#include "Blam/Cache/TagGroups/object_definition.hpp"

struct real_hsv_color
{
	float hue;
	float saturation;
	float value;
};
CHECK_STRUCT_SIZE(real_hsv_color, 12);


void __cdecl real_rgb_color_to_real_hsv_color(const real_color_rgb* rgb, real_hsv_color* hsv);
void __cdecl real_hsv_color_to_real_rgb_color(const real_hsv_color* hsv, real_color_rgb* rgb);
void __cdecl rgb_colors_interpolate(real_color_rgb* rgb_result,
    s_object_group_definition::s_change_colors_block::s_change_colors_functions_block::e_colors_function_scale_flags scale_flags,
    const real_color_rgb* rgb_lower_bound,
    const real_color_rgb* rgb_upper_bound,
    float multiplier);
