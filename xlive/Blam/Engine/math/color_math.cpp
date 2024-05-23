#include "stdafx.h"
#include "color_math.h"

pixel32 real_argb_color_to_pixel32(const real_argb_color* color)
{
    pixel32 result = { D3DCOLOR_ARGB(
        (int32)(color->alpha * 255), 
        (int32)(color->red * 255),
        (int32)(color->green * 255),
        (int32)(color->blue * 255)
    ) };
    return result;
}
