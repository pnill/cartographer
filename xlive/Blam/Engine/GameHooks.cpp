#include "stdafx.h"
#include "game/cheats.h"
#include "math/color_math.h"
#include "math/random_math.h"

void ApplyGameHooks()
{
	cheats::ApplyHooks();
	color_math::ApplyHooks();
	random_math::ApplyHooks();
}
