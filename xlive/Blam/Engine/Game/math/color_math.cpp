#include "stdafx.h"
#include "color_math.h"

#include "Util/Hooks/Hook.h"

typedef void(__cdecl* real_rgb_colour_to_real_hsv_color_t)(const real_color_rgb* rgb, real_color_hsv* hsv);
real_rgb_colour_to_real_hsv_color_t p_real_rgb_colour_to_real_hsv_color;

typedef void(__cdecl* real_hsv_color_to_real_rgb_color_t)(const real_color_hsv* hsv, real_color_rgb* rgb);
real_hsv_color_to_real_rgb_color_t p_real_hsv_color_to_real_rgb_color;

typedef D3DCOLOR(__cdecl* real_argb_colour_to_d3d_color_t)(real_color_argb* color);
real_argb_colour_to_d3d_color_t p_real_argb_colour_to_d3d_color;

typedef D3DCOLOR(__cdecl* real_alpha_to_d3d_alpha_t)(float alpha);
real_alpha_to_d3d_alpha_t p_real_alpha_to_d3d_alpha;

namespace color_math
{
    void __cdecl real_rgb_colour_to_real_hsv_color(const real_color_rgb* rgb, real_color_hsv* hsv)
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

    void __cdecl real_hsv_color_to_real_rgb_color(const real_color_hsv* hsv, real_color_rgb* rgb)
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

    D3DCOLOR __cdecl real_argb_colour_to_d3d_color(const real_color_argb* color)
    {
        byte alpha = (byte)(color->alpha * 255);
        byte red = (byte)(color->color.red * 255);
        byte green = (byte)(color->color.green * 255);
        byte blue = (byte)(color->color.blue * 255);
        return D3DCOLOR_ARGB(alpha, red, green, blue);
    }

    D3DCOLOR __cdecl real_alpha_to_d3d_alpha(const float alpha)
    {
        byte d3d_alpha = (byte)alpha * 255;
        return D3DCOLOR_ARGB(d3d_alpha, 0, 0, 0);
    }

    void ApplyHooks()
    {
        DETOUR_ATTACH(p_real_rgb_colour_to_real_hsv_color, Memory::GetAddress<real_rgb_colour_to_real_hsv_color_t>(0x75D34, 0x720C0F), real_rgb_colour_to_real_hsv_color);
        DETOUR_ATTACH(p_real_hsv_color_to_real_rgb_color, Memory::GetAddress<real_hsv_color_to_real_rgb_color_t>(0x75E44, 0x72D1F), real_hsv_color_to_real_rgb_color);
        DETOUR_ATTACH(p_real_argb_colour_to_d3d_color, Memory::GetAddress<real_argb_colour_to_d3d_color_t>(0x7602F, 0x72EA1), real_argb_colour_to_d3d_color);
        DETOUR_ATTACH(p_real_alpha_to_d3d_alpha, Memory::GetAddress<real_alpha_to_d3d_alpha_t>(0x7617E), real_alpha_to_d3d_alpha);
    }
}
