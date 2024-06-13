#pragma once

/* structures */
struct pixel32
{
	D3DCOLOR color;
};
ASSERT_STRUCT_SIZE(pixel32, sizeof(uint8) * 4);

/* channel intensity is represented on a 0 to 1 scale */
union real_argb_color
{
	real32 v[4];
	struct { real32 alpha, red, green, blue; };
};
ASSERT_STRUCT_SIZE(real_argb_color, sizeof(real32) * 4);

union real_rgb_color
{
	real32 v[3];
	struct { real32 red, green, blue; };
};
ASSERT_STRUCT_SIZE(real_rgb_color, sizeof(real32) * 3);

/* constants */

const pixel32 global_white_pixel32 = { D3DCOLOR_ARGB(255, 255, 255, 255) };
const pixel32 global_yellow_pixel32 = { D3DCOLOR_ARGB(255, 255, 255, 0) };

// h1/reach puts these in cseries.obj because it is the first file that includes them? or defines them?
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

const real_rgb_color* const global_real_rgb_white = (const real_rgb_color* const)&global_real_argb_color_table[0].red;
const real_rgb_color* const global_real_rgb_grey = (const real_rgb_color* const)&global_real_argb_color_table[1].red;
const real_rgb_color* const global_real_rgb_black = (const real_rgb_color* const)&global_real_argb_color_table[2].red;
const real_rgb_color* const global_real_rgb_red = (const real_rgb_color* const)&global_real_argb_color_table[3].red;
const real_rgb_color* const global_real_rgb_green = (const real_rgb_color* const)&global_real_argb_color_table[4].red;
const real_rgb_color* const global_real_rgb_blue = (const real_rgb_color* const)&global_real_argb_color_table[5].red;
const real_rgb_color* const global_real_rgb_cyan = (const real_rgb_color* const)&global_real_argb_color_table[6].red;
const real_rgb_color* const global_real_rgb_yellow = (const real_rgb_color* const)&global_real_argb_color_table[7].red;
const real_rgb_color* const global_real_rgb_magenta = (const real_rgb_color* const)&global_real_argb_color_table[8].red;
const real_rgb_color* const global_real_rgb_pink = (const real_rgb_color* const)&global_real_argb_color_table[9].red;
const real_rgb_color* const global_real_rgb_lightblue = (const real_rgb_color* const)&global_real_argb_color_table[10].red;
const real_rgb_color* const global_real_rgb_orange = (const real_rgb_color* const)&global_real_argb_color_table[11].red;
const real_rgb_color* const global_real_rgb_purple = (const real_rgb_color* const)&global_real_argb_color_table[12].red;
const real_rgb_color* const global_real_rgb_aqua = (const real_rgb_color* const)&global_real_argb_color_table[13].red;
const real_rgb_color* const global_real_rgb_darkgreen = (const real_rgb_color* const)&global_real_argb_color_table[14].red;
const real_rgb_color* const global_real_rgb_salmon = (const real_rgb_color* const)&global_real_argb_color_table[15].red;
const real_rgb_color* const global_real_rgb_violet = (const real_rgb_color* const)&global_real_argb_color_table[16].red;

/* public code */

pixel32 real_argb_color_to_pixel32(const real_argb_color* color);
