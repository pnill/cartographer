#pragma once

struct pixel32
{
	byte alpha;
	byte red;
	byte green;
	byte blue;
};
CHECK_STRUCT_SIZE(pixel32, sizeof(BYTE) * 4);

/* channel intensity is represented on a 0 to 1 scale */
union real_color_argb
{
	real32 v[4];
	struct { real32 alpha, red, green, blue; };
};
CHECK_STRUCT_SIZE(real_color_argb, sizeof(real32) * 4);

union real_color_rgb
{
	real32 v[3];
	struct { real32 red, green, blue; };
};
CHECK_STRUCT_SIZE(real_color_rgb, sizeof(real32) * 3);
