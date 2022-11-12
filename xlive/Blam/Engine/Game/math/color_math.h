#pragma once
#include "Blam/Common/Common.h"

/* channel intensity is represented on a 0 to 1 scale */
struct real_color_argb
{
	float alpha = 1.0f;
	float red = 1.0f;
	float green = 1.0f;
	float blue = 1.0f;

	real_color_argb() {}

	real_color_argb(float _alpha, float _red, float _green, float _blue) :
		alpha(_alpha),
		red(_red),
		green(_green),
		blue(_blue)
	{}
};
CHECK_STRUCT_SIZE(real_color_argb, sizeof(float) * 4);

struct real_color_rgb
{
	float red = 1.0f;
	float green = 1.0f;
	float blue = 1.0f;

	real_color_rgb() {}

	real_color_rgb(float _red, float _green, float _blue) :
		red(_red),
		green(_green),
		blue(_blue)
	{}

	real_color_rgb(const real_color_argb& colour) :
		red(colour.red),
		green(colour.green),
		blue(colour.blue)
	{}

	real_color_argb as_rgba(float _alpha = 1.0f)
	{
		real_color_argb converted;
		converted.alpha = _alpha;
		converted.red = red;
		converted.green = green;
		converted.blue = blue;
		return converted;
	}
};
CHECK_STRUCT_SIZE(real_color_rgb, sizeof(float) * 3);

struct real_color_hsv
{
	float hue = 1.0;
	float saturation = 1.0;
	float value = 1.0;
};
CHECK_STRUCT_SIZE(real_color_hsv, sizeof(float) * 3);

namespace color_math
{
	void ApplyHooks();
}
