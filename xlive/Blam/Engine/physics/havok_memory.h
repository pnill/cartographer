#pragma once

/* headers */

#include <havok/hkbase/memory/hkMemory.h>

/* enums */

enum e_memory_allocator_type
{
	_memory_allocator_type_none= 0,
	_memory_allocator_type_startup,
	_memory_allocator_type_runtime,
	k_memory_allocator_type_count,
};

/* structures */

struct s_runtime_pool_memory_usage
{
	struct s_bucket_stats
	{
		int32 bucket_size;
		int32 total_elements;
		int32 elements_in_use;
	};

	int32 runtime_memory;
	int32 total_pages;
	int32 pages_in_use;
	int32 page_size;
	int32 current_page_free_bytes;
	real32 memory_usage_fraction;
	s_bucket_stats buckets[16];
	int32 debug_memory_size;
	int32 bucket_count;
	int8 gap[64];
};

struct s_startup_pool_memory_usage
{
	int32 memory_total;
	int32 memory_in_use;
	int32 allocation_count;
	int32 debug_memory_size;
};

struct s_overflow_pool_memory_usage
{
	int32 memory_total;
	int32 memory_in_use;
	int32 allocation_count;
	int32 debug_memory_size;
};

struct s_2nd_overflow_pool_memory_usage
{
	int32 memory_total;
	int32 memory_in_use;
	int32 maximum_allocations;
	int32 allocation_count;
	int32 debug_memory_size;
};

struct s_havok_memory_usage
{
	s_runtime_pool_memory_usage runtime_pool_memory_usage;
	s_startup_pool_memory_usage startup_pool_memory_usage;
	s_overflow_pool_memory_usage overflow_pool_memory_usage;
	s_2nd_overflow_pool_memory_usage second_overflow_pool_memory_usage;
};
ASSERT_STRUCT_SIZE(s_havok_memory_usage, 340);

/* prototypes */

void __cdecl havok_memory_garbage_collect(void);
