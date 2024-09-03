#include "stdafx.h"
#include "color_math.h"

const real_argb_color global_real_argb_color_table[] =
{
	{ {  1.f,	1.f,	1.f,	1.f } },
	{ {  1.f,	.5f,	.5f,	.5f } },
	{ {  1.f,	.0f,	.0f,	.0f } },
	{ {  1.f,	1.f,	.0f,	.0f } },
	{ {  1.f,	.0f,	1.f,	.0f } },
	{ {  1.f,	.0f,	.0f,	1.f } },
	{ {  1.f,	.0f,	1.f,	1.f } },
	{ {  1.f,	1.f,	1.f,	.0f } },
	{ {  1.f,	1.f,	.0f,	1.f } },
	{ {  1.f,	1.f,	.41f,	.7f } },
	{ {  1.f,	.39f,	.58f,	.93f } },
	{ {  1.f,	1.f,	.5f,	.0f } },
	{ {  1.f,	.44f,	.05f,	.43f } },
	{ {  1.f,	.5f,	1.f,	.83f } },
	{ {  1.f,	.0f,	.39f,	.0f } },
	{ {  1.f,	1.f,	.63f,	.48f } },
	{ {  1.f,	.81f,	.13f,	.56f } }
};

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

void pixel32_to_real_rgb_color(pixel32 pixel_color, real_rgb_color* out_color)
{
    INVOKE(0x75FE2, 0, pixel32_to_real_rgb_color, pixel_color, out_color);
    return;
}

pixel32 real_alpha_to_pixel32(real32 alpha)
{
    ASSERT(alpha >= 0.f && alpha <= 1.f);
    pixel32 color = { (int32)(alpha * 255.f) << 24 };
    return color;
}
