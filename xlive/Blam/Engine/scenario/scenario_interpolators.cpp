#include "stdafx.h"
#include "scenario_interpolators.h"

/* public code */

real32 __cdecl scenario_interpolator_get_result(int32 index)
{
	return INVOKE(0x9F301, 0x0, scenario_interpolator_get_result, index);
}
