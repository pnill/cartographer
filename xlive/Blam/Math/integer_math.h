#pragma once

#include "Blam/Common/Common.h"

//use this for all base integer math related structs and implementations
struct point2d
{
	short x;
	short y;
};
CHECK_STRUCT_SIZE(point2d, sizeof(short) * 2);

struct point3d
{
	short x;
	short y;
	short z;
};
CHECK_STRUCT_SIZE(point3d, sizeof(short) * 3);

struct short_bounds
{
	short lower;
	short upper;
};
CHECK_STRUCT_SIZE(short_bounds, sizeof(short) * 2);

struct rect2d
{
	short top;
	short left;
	short bottom;
	short right;
};
CHECK_STRUCT_SIZE(rect2d, sizeof(short) * 4);

struct byte_color_argb
{
	BYTE alpha = 255;
	BYTE red = 255;
	BYTE green = 255;
	BYTE blue = 255;

	byte_color_argb() {}

	byte_color_argb(BYTE _alpha, BYTE _red, BYTE _green, BYTE _blue) :
		alpha(_alpha),
		red(_red),
		green(_green),
		blue(_blue)
	{}

};
CHECK_STRUCT_SIZE(byte_color_argb, sizeof(BYTE) * 4);