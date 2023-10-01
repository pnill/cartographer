#pragma once
#include "Blam/Cache/DataTypes/BlamTag.h"

struct alignas(4) s_tag_data_iterator
{
	DWORD dword0;
	DWORD dword4;
	datum current_tag_datum;
	datum next_tag_datum;
	blam_tag tag_type;
	datum get_next_datum();
};
CHECK_STRUCT_SIZE(s_tag_data_iterator, 20);

struct cache_file_tag_instance
{
	blam_tag tag_group;
	int32 tag_index;
	uint32 data_offset;
	uint32 size;
};
CHECK_STRUCT_SIZE(cache_file_tag_instance, 16);

cache_file_tag_instance* global_tag_instances_get(void);
void cache_file_map_clear_all_failures(void);

// Get tag data from tag index
void* __cdecl tag_get_fast(datum tag_index);