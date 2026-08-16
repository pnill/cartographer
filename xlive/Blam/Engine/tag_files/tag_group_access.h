#pragma once
#include "cache/cache_files.h"

/* structures */

struct tag_data
{
	int32 size;
	int32 data;
};
ASSERT_STRUCT_SIZE(tag_data, 8);

struct tag_reference
{
	tag_group group;
	datum index;
};
ASSERT_STRUCT_SIZE(tag_reference, 8);


/* public code */

inline struct cache_file_tag_instance const* cache_file_tag_instance_try_and_get_unsafe(
	int32 tag_index)
{
	short absolute_index;
	short identifier;
	cache_file_tag_instance const* tag_instance;

	s_cache_file_memory_globals const* g_cache_file_memory_globals = cache_file_memory_globals_get();
	cache_file_tag_instance const * global_tag_instances = global_tag_instances_get();

	ASSERT(g_cache_file_memory_globals->tags_loaded);
	ASSERT(global_tag_instances);

	absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_index);
	identifier = DATUM_INDEX_TO_IDENTIFIER(tag_index);

	vassert(identifier, "%08x is an invalid absolute tag index", tag_index);
	vassert(absolute_index < 32768 && absolute_index < g_cache_file_memory_globals->tags_header->tag_count, "%08x is outside the range of valid tag indices", tag_index);

	tag_instance = &global_tag_instances[absolute_index];

	vassert(tag_instance->tag_index==tag_index, "tag index %08x should be %08x", tag_index, tag_instance->tag_index);

	return tag_instance;
}

inline void* tag_data_get_address(
	tag_data const* data)
{
	ASSERT(data);

	void* result = NULL;
	
	if (data->data != NONE)
	{
		result = (void*)(data->data + *tag_data_get());
	}
	
	return result;
}

inline void* tag_block_get_address(
	s_tag_block const* block)
{
	ASSERT(block);

	void* result = NULL;

	if (block->data != NONE)
	{
		result = (void*)(block->data + *tag_data_get());
	}

	return result;
}
