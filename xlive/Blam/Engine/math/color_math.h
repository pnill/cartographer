#pragma once

/* typedefs */

typedef uint32 pixel32;

/* macros */

#define PIXEL32_ARGB(a,r,g,b) ((pixel32)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define PIXEL32_RGBA(r,g,b,a) PIXEL32_ARGB(a,r,g,b)

/* structures */

union real_rgb_color
{
	real32 n[3];
	struct { real32 red, green, blue; };
};
ASSERT_STRUCT_SIZE(real_rgb_color, sizeof(real32) * 3);

/* channel intensity is represented on a 0 to 1 scale */
union real_argb_color
{
	real32 n[4];
	struct
	{
		real32 alpha;
		union
		{
			struct
			{
				real32 red, green, blue;
			};
			real_rgb_color rgb;
		};
	};
};
ASSERT_STRUCT_SIZE(real_argb_color, sizeof(real32) * 4);

/* constants */

extern const real_argb_color* const global_real_argb_white;
extern const real_argb_color* const global_real_argb_grey;
extern const real_argb_color* const global_real_argb_black;
extern const real_argb_color* const global_real_argb_red;
extern const real_argb_color* const global_real_argb_green;
extern const real_argb_color* const global_real_argb_blue;
extern const real_argb_color* const global_real_argb_cyan;
extern const real_argb_color* const global_real_argb_yellow;
extern const real_argb_color* const global_real_argb_magenta;
extern const real_argb_color* const global_real_argb_pink;
extern const real_argb_color* const global_real_argb_lightblue;
extern const real_argb_color* const global_real_argb_orange;
extern const real_argb_color* const global_real_argb_purple;
extern const real_argb_color* const global_real_argb_aqua;
extern const real_argb_color* const global_real_argb_darkgreen;
extern const real_argb_color* const global_real_argb_salmon;
extern const real_argb_color* const global_real_argb_violet;

extern const real_rgb_color* const global_real_rgb_white;
extern const real_rgb_color* const global_real_rgb_grey;
extern const real_rgb_color* const global_real_rgb_black;
extern const real_rgb_color* const global_real_rgb_red;
extern const real_rgb_color* const global_real_rgb_green;
extern const real_rgb_color* const global_real_rgb_blue;
extern const real_rgb_color* const global_real_rgb_cyan;
extern const real_rgb_color* const global_real_rgb_yellow;
extern const real_rgb_color* const global_real_rgb_magenta;
extern const real_rgb_color* const global_real_rgb_pink;
extern const real_rgb_color* const global_real_rgb_lightblue;
extern const real_rgb_color* const global_real_rgb_orange;
extern const real_rgb_color* const global_real_rgb_purple;
extern const real_rgb_color* const global_real_rgb_aqua;
extern const real_rgb_color* const global_real_rgb_darkgreen;
extern const real_rgb_color* const global_real_rgb_salmon;
extern const real_rgb_color* const global_real_rgb_violet;

extern const pixel32 global_white_pixel32;
extern const pixel32 global_yellow_pixel32;

/* public code */

pixel32 real_argb_color_to_pixel32(const real_argb_color* color);

void pixel32_to_real_rgb_color(pixel32 pixel_color, real_rgb_color* out_color);

pixel32 real_alpha_to_pixel32(real32 alpha);
