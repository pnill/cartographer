#pragma once
#include "memory/data.h"
#include "geometry/geometry_block.h"

struct pc_geometry_cache_geometry_block
{
	int unk_1;
	int unk_2;
	int unk_3;
};

struct pc_geometry_cache_globals
{
	s_data_array* geometry_blocks;
	uint32 cache_base_address;
	uint32 cache_base_address_allocation_size;
};

enum e_pc_geometry_cache_preload_flags : uint8
{
	_pc_geometry_cache_preload_blocking = FLAG(0),
	_pc_geometry_cache_preload_flag_2 = FLAG(1),
	_pc_geometry_cache_preload_flag_3 = FLAG(2),
	_pc_geometry_cache_preload_flag_4 = FLAG(3),
	_pc_geometry_cache_preload_flag_5 = FLAG(4)
};

void __cdecl pc_geometry_cache_block_count_clear(void);

bool __cdecl pc_geometry_cache_preload_geometry(geometry_block_info* geometry_block_info, e_pc_geometry_cache_preload_flags preload_flags);