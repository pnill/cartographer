#pragma once

struct pixel32
{
	uint8 alpha;
	uint8 red;
	uint8 green;
	uint8 blue;
};
CHECK_STRUCT_SIZE(pixel32, sizeof(uint8) * 4);

/* channel intensity is represented on a 0 to 1 scale */
union real_argb_color
{
	real32 v[4];
	struct { real32 alpha, red, green, blue; };
};
CHECK_STRUCT_SIZE(real_argb_color, sizeof(real32) * 4);

union real_rgb_color
{
	real32 v[3];
	struct { real32 red, green, blue; };
};
CHECK_STRUCT_SIZE(real_rgb_color, sizeof(real32) * 3);

const pixel32 global_black_pixel32 = { 255,255,255,255 };
