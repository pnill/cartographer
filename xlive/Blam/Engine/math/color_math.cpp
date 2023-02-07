#include "stdafx.h"
#include "color_math.h"

void __cdecl real_rgb_color_to_real_hsv_color(const real_color_rgb* rgb, real_hsv_color* hsv)
{
    float max = fmax(rgb->red, fmax(rgb->green, rgb->blue));	// max of r, g, b
    float min = fmin(rgb->red, fmin(rgb->green, rgb->blue));	// min of r, g, b
    float difference = max - min;								// difference of cmax and cmin.

    // Calculate Hue
    hsv->hue =
        max == rgb->red ? 0.16666667 * ((rgb->green - rgb->blue) / difference) :			// If cmax equals red then compute Hue
        max == rgb->green ? 0.16666667 * (((rgb->blue - rgb->red) / difference) + 2) :		// If cmax equals green then compute Hue
        max == rgb->blue ? 0.16666667 * (((rgb->red - rgb->green) / difference) + 4) : 0;	// If cmax equals blue then compute Hue

    // If Hue is less than zero add 1 to it
    (hsv->hue < 0.0 ? hsv->hue += 1 : 0);

    // Calculate Saturation
    hsv->saturation = (max == 0 ? 0 : (difference / max));

    // Set hue to zero if saturation is 0
    hsv->hue = (max == 0 ? 0 : hsv->hue);

    // Calculate Value
    hsv->value = max;
}

void __cdecl real_hsv_color_to_real_rgb_color(const real_hsv_color* hsv, real_color_rgb* rgb)
{
    float normalized_hue = hsv->hue * 6;
    int whole_normalized_hue = (int)normalized_hue;
    float hue_difference = normalized_hue - (float)whole_normalized_hue;
    float alpha = (1.0f - hsv->saturation) * hsv->value;
    float beta = (1.0f - (hsv->saturation * hue_difference)) * hsv->value;
    float gamma = (1.0f - ((1.0f - hue_difference) * hsv->saturation)) * hsv->value;

    if (hsv->saturation == 0.0)
    {
        rgb->blue = hsv->value;
        rgb->green = hsv->value;
        rgb->red = hsv->value;
    }
    else
    {
        switch (whole_normalized_hue)
        {
        case 0:
            rgb->red = hsv->value;
            rgb->green = gamma;
            rgb->blue = alpha;
            break;
        case 1:
            rgb->red = beta;
            rgb->green = hsv->value;
            rgb->blue = alpha;
            break;
        case 2:
            rgb->red = alpha;
            rgb->green = hsv->value;
            rgb->blue = gamma;
            break;
        case 3:
            rgb->red = alpha;
            rgb->green = beta;
            rgb->blue = hsv->value;
            break;
        case 4:
            rgb->red = gamma;
            rgb->green = alpha;
            rgb->blue = hsv->value;
            break;
        default:
            rgb->red = hsv->value;
            rgb->green = alpha;
            rgb->blue = beta;
            break;
        }
    }
}

void __cdecl rgb_colors_interpolate(real_color_rgb* rgb_result,
    s_object_group_definition::s_change_colors_block::s_change_colors_functions_block::e_colors_function_scale_flags scale_flags,
    const real_color_rgb* rgb_lower_bound,
    const real_color_rgb* rgb_upper_bound,
    float multiplier)
{
    float one_minus_multiplier;
    float upper_hue;
    float lower_hue;
    float v8;
    real_hsv_color hsv_lower_bound;
    real_hsv_color hsv_upper_bound;
    real_hsv_color hsv_result;


    one_minus_multiplier = 1.0f - multiplier;
    if ((scale_flags & s_object_group_definition::s_change_colors_block::s_change_colors_functions_block::e_colors_function_scale_flags::blend_in_hsv) != 0)
    {
        real_rgb_color_to_real_hsv_color(rgb_lower_bound, &hsv_lower_bound);
        real_rgb_color_to_real_hsv_color(rgb_upper_bound, &hsv_upper_bound);
        upper_hue = hsv_upper_bound.hue;
        lower_hue = hsv_lower_bound.hue;
        if (fabs(hsv_lower_bound.hue - hsv_upper_bound.hue) > 0.5f != ((scale_flags >> 1) & 1))
        {
            if (hsv_upper_bound.hue <= hsv_lower_bound.hue)
                upper_hue = hsv_upper_bound.hue + 1.0f;
            else
                lower_hue = hsv_lower_bound.hue + 1.0f;
        }
        v8 = (lower_hue * one_minus_multiplier) + (upper_hue * multiplier);
        hsv_result.hue = v8;

        if (v8 > 1.0f) { hsv_result.hue = v8 - 1.0f; }
        hsv_result.saturation = (hsv_lower_bound.saturation * one_minus_multiplier) + (hsv_upper_bound.saturation * multiplier);
        hsv_result.value = (hsv_lower_bound.value * one_minus_multiplier) + (hsv_upper_bound.value * multiplier);
        real_hsv_color_to_real_rgb_color(&hsv_result, rgb_result);
    }
    else
    {
        rgb_result->red = (rgb_lower_bound->red * one_minus_multiplier) + (rgb_upper_bound->red * multiplier);
        rgb_result->green = (rgb_lower_bound->green * one_minus_multiplier) + (rgb_upper_bound->green * multiplier);
        rgb_result->blue = (rgb_lower_bound->blue * one_minus_multiplier) + (rgb_upper_bound->blue * multiplier);
    }
}
