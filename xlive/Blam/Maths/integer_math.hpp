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