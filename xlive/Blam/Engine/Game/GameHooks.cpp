#include "stdafx.h"
#include "math/color_math.h"
#include "math/random_math.h"

void ApplyGameHooks()
{
	color_math::ApplyHooks();
	random_math::ApplyHooks();
}
