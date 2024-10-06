#include "stdafx.h"
#include "render_patchy_fog.h"

/* public code */

s_patchy_fog_globals* patchy_fog_globals_get(void)
{
	return Memory::GetAddress<s_patchy_fog_globals*>(0xA567C0);
}
