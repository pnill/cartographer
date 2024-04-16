#pragma once

union point2d
{
	int16 v[2];
	struct { int16 x, y; };
};
ASSERT_STRUCT_SIZE(point2d, sizeof(int16) * 2);

union short_bounds
{
	int16 v[2];
	struct { int16 lower, upper; };
};
ASSERT_STRUCT_SIZE(short_bounds, sizeof(int16) * 2);

union rectangle2d
{
	int16 v[4];
	struct { int16 top, left, bottom, right; };
};
ASSERT_STRUCT_SIZE(rectangle2d, sizeof(int16) * 4);

static BLAM_MATH_INL int16 rectangle2d_width(const rectangle2d* rect)
{
	return rect->right - rect->left;
}

static BLAM_MATH_INL int16 rectangle2d_height(const rectangle2d* rect)
{
	return rect->bottom - rect->top;
}
