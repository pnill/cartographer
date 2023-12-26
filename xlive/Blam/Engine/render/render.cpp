#include "stdafx.h"
#include "render.h"

real32 get_current_render_time()
{
	return *Memory::GetAddress<real32*>(0x4E6968);
}
