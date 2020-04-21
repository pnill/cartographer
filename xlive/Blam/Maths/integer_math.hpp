#pragma once
//use this for all base integer math related structs and implementations

#pragma once
#include "..\Blam\Cache\DataTypes\Macros.h"

struct point2d
{
	short x;
	short y;
};
CHECK_STRUCT_SIZE(point2d, 2 * 2);

struct short_bounds
{
	short lower;
	short upper;
};

struct rect2d
{
	WORD top;
	WORD left;
	WORD bottom;
	WORD right;
};
struct byte_color_argb
{
	
	byte alpha = 255;
	byte red = 255;
	byte green = 255;
	byte blue = 255;

	byte_color_argb() {}

	byte_color_argb(byte _alpha, byte _red, byte _green, byte _blue) :
		alpha(_alpha),
		red(_red),
		green(_green),
		blue(_blue)
	{}

};
CHECK_STRUCT_SIZE(byte_color_argb, 1 * 4);