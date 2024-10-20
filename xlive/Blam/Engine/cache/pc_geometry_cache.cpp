#include "stdafx.h"
#include "pc_geometry_cache.h"

/* public code */

void __cdecl geometry_cache_new(void)
{
	INVOKE(0x26526C, 0x0, geometry_cache_new);
	return;
}

void __cdecl pc_geometry_cache_block_count_clear(void)
{
	INVOKE(0x264F31, 0x0, pc_geometry_cache_block_count_clear);
	return;
}

bool __cdecl pc_geometry_cache_preload_geometry(geometry_block_info* geometry_block_info,
	e_pc_geometry_cache_preload_flags preload_flags)
{
	return INVOKE(0x2652BC, 0, pc_geometry_cache_preload_geometry, geometry_block_info, preload_flags);
}

void geometry_cache_block_for_one_frame(int32 a1)
{
	/* does nothing on pc */
	return;
}
