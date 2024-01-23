#pragma once
#include "Blam/Cache/DataTypes/BlamTag.h"

#define MAXIMUM_SIMULTANEOUS_TAG_INSTANCES_TOTAL 20000

struct tag_iterator
{
	int32 field_0;
	int32 field_4;
	datum current_tag_index;
	datum next_tag_index;
	blam_tag tag_type;
};
CHECK_STRUCT_SIZE(tag_iterator, 20);

struct cache_file_tag_instance
{
	blam_tag tag_group;
	int32 tag_index;
	uint32 data_offset;
	uint32 size;
};
CHECK_STRUCT_SIZE(cache_file_tag_instance, 16);

cache_file_tag_instance* global_tag_instances_get(void);

tag_iterator* tag_iterator_new(tag_iterator* itr, blam_tag::tag_group_type type);

datum __cdecl tag_iterator_next(tag_iterator* itr);

void cache_file_map_clear_all_failures(void);

// Get tag data from tag index
void* __cdecl tag_get_fast(datum tag_index);