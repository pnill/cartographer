#include "stdafx.h"
#include "math.h"

int32 blam_ticks_real_to_integer(real32 ticks_real)
{
	int32 tick_val_integer = static_cast<int32>(ticks_real + (ticks_real < 0.0f ? -0.5f : 0.5f));
	return tick_val_integer;
}