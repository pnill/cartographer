#pragma once
#include "memory/data.h"

struct s_lruv_cache_block
{
	int16 field_0;
	int16 pad;
	int32 page_count;
	int32 first_page_index;
	int32 next_block_index;
	int32 previous_block_index;
};
CHECK_STRUCT_SIZE(s_lruv_cache_block, 20);

struct s_lruv_cache
{
	char data_name[32];
	int32 purged_block_proc;
	int32 locked_block_proc;
	int32 proc_28;
	int32 field_2C;
	int32 field_30;
	int32 page_size_bits;
	int32 field_38;
	int32 first_block_index;
	int32 last_block_index;
	int32 pages[8];
	s_data_array* next_block;
	int8 signature[4];
	void* allocate_proc;
};
CHECK_STRUCT_SIZE(s_lruv_cache, 112);
