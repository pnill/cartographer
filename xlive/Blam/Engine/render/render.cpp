#include "stdafx.h"
#include "render.h"

real64 get_current_render_time(void)
{
	return *Memory::GetAddress<real64*>(0x4E6968);
}
