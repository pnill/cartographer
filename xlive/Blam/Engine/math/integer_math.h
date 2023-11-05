#pragma once



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

	int width() const
	{
		return right - left;
	}

	int height() const
	{
		return bottom - top;
	}

	void add(const rect2d& other)
	{
		top += other.top;
		left += other.left;
		bottom += other.bottom;
		right += other.right;
	}

	void subtract(const rect2d& other)
	{
		top -= other.top;
		left -= other.left;
		bottom -= other.bottom;
		right -= other.right;
	}
};
CHECK_STRUCT_SIZE(rect2d, sizeof(short) * 4);

