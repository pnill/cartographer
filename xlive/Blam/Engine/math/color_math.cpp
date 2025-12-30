#include "stdafx.h"
#include "color_math.h"

/* constants */

static const real_argb_color global_real_argb_color_table[] =
{
	{ 1.f, 1.f,		1.f,	1.f  },
	{ 1.f, .5f,		.5f,	.5f  },
	{ 1.f, .0f,		.0f,	.0f  },
	{ 1.f, 1.f,		.0f,	.0f  },
	{ 1.f, .0f,		1.f,	.0f  },
	{ 1.f, .0f,		.0f,	1.f  },
	{ 1.f, .0f,		1.f,	1.f  },
	{ 1.f, 1.f,		1.f,	.0f  },
	{ 1.f, 1.f,		.0f,	1.f  },
	{ 1.f, 1.f,		.41f,	.7f  },
	{ 1.f, .39f,	.58f,	.93f },
	{ 1.f, 1.f,		.5f,	.0f },
	{ 1.f, .44f,	.05f,	.43f },
	{ 1.f, .5f,		1.f,	.83f },
	{ 1.f, .0f,		.39f,	.0f },
	{ 1.f, 1.f,		.63f,	.48f },
	{ 1.f, .81f,	.13f,	.56f }
};

const real_argb_color* const global_real_argb_white = &global_real_argb_color_table[0];
const real_argb_color* const global_real_argb_grey = &global_real_argb_color_table[1];
const real_argb_color* const global_real_argb_black = &global_real_argb_color_table[2];
const real_argb_color* const global_real_argb_red = &global_real_argb_color_table[3];
const real_argb_color* const global_real_argb_green = &global_real_argb_color_table[4];
const real_argb_color* const global_real_argb_blue = &global_real_argb_color_table[5];
const real_argb_color* const global_real_argb_cyan = &global_real_argb_color_table[6];
const real_argb_color* const global_real_argb_yellow = &global_real_argb_color_table[7];
const real_argb_color* const global_real_argb_magenta = &global_real_argb_color_table[8];
const real_argb_color* const global_real_argb_pink = &global_real_argb_color_table[9];
const real_argb_color* const global_real_argb_lightblue = &global_real_argb_color_table[10];
const real_argb_color* const global_real_argb_orange = &global_real_argb_color_table[11];
const real_argb_color* const global_real_argb_purple = &global_real_argb_color_table[12];
const real_argb_color* const global_real_argb_aqua = &global_real_argb_color_table[13];
const real_argb_color* const global_real_argb_darkgreen = &global_real_argb_color_table[14];
const real_argb_color* const global_real_argb_salmon = &global_real_argb_color_table[15];
const real_argb_color* const global_real_argb_violet = &global_real_argb_color_table[16];

const real_rgb_color* const global_real_rgb_white = &global_real_argb_color_table[0].rgb;
const real_rgb_color* const global_real_rgb_grey = &global_real_argb_color_table[1].rgb;
const real_rgb_color* const global_real_rgb_black = &global_real_argb_color_table[2].rgb;
const real_rgb_color* const global_real_rgb_red = &global_real_argb_color_table[3].rgb;
const real_rgb_color* const global_real_rgb_green = &global_real_argb_color_table[4].rgb;
const real_rgb_color* const global_real_rgb_blue = &global_real_argb_color_table[5].rgb;
const real_rgb_color* const global_real_rgb_cyan = &global_real_argb_color_table[6].rgb;
const real_rgb_color* const global_real_rgb_yellow = &global_real_argb_color_table[7].rgb;
const real_rgb_color* const global_real_rgb_magenta = &global_real_argb_color_table[8].rgb;
const real_rgb_color* const global_real_rgb_pink = &global_real_argb_color_table[9].rgb;
const real_rgb_color* const global_real_rgb_lightblue = &global_real_argb_color_table[10].rgb;
const real_rgb_color* const global_real_rgb_orange = &global_real_argb_color_table[11].rgb;
const real_rgb_color* const global_real_rgb_purple = &global_real_argb_color_table[12].rgb;
const real_rgb_color* const global_real_rgb_aqua = &global_real_argb_color_table[13].rgb;
const real_rgb_color* const global_real_rgb_darkgreen = &global_real_argb_color_table[14].rgb;
const real_rgb_color* const global_real_rgb_salmon = &global_real_argb_color_table[15].rgb;
const real_rgb_color* const global_real_rgb_violet = &global_real_argb_color_table[16].rgb;

const pixel32 global_white_pixel32 = D3DCOLOR_ARGB(255, 255, 255, 255);
const pixel32 global_yellow_pixel32 = D3DCOLOR_ARGB(255, 255, 255, 0);

/* public code */

pixel32 real_argb_color_to_pixel32(const real_argb_color* color)
{
	return D3DCOLOR_ARGB(
		(int32)(color->alpha * 255),
		(int32)(color->red * 255),
		(int32)(color->green * 255),
		(int32)(color->blue * 255));
}

void pixel32_to_real_rgb_color(pixel32 pixel_color, real_rgb_color* out_color)
{
	out_color->red = (real32)((pixel_color >> 16) & 255) / 255.f;
	out_color->green = (real32)((pixel_color >> 8) & 255) / 255.f;
	out_color->blue = (real32)((pixel_color >> 0) & 255) / 255.f;
	return;
}

pixel32 real_alpha_to_pixel32(real32 alpha)
{
	ASSERT(alpha >= 0.f && alpha <= 1.f);
	pixel32 color = (int32)(alpha * 255.f) << 24;
	return color;
}
