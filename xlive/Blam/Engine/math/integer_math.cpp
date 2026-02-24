#include "stdafx.h"
#include "integer_math.h"

/* public code */

void rectangle2d_to_rect(
	const rectangle2d* rect2d,
	RECT* rect)
{
	const int16 width = rectangle2d_width(rect2d);
	const int16 height = rectangle2d_height(rect2d);
	*rect =
	{
		rect2d->left,
		rect2d->top,
		rect2d->left + width,
		rect2d->top + height
	};

	return;
}
