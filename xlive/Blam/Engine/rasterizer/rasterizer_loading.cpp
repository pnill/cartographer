#include "stdafx.h"
#include "rasterizer_loading.h"

bool __cdecl loading_screen_in_progress(void)
{
	return INVOKE(0x26A05A, 0x0, loading_screen_in_progress);
}
