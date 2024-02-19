#include "stdafx.h"
#include "cache_files.h"

#include "Blam/Engine/game/game_globals.h"

s_cache_header* cache_files_get_header(void)
{
	return Memory::GetAddress<s_cache_header*>(0x47CD68, 0x4A29D0);
}

s_tags_header* cache_files_get_tags_header(void)
{
	return tags::get_at_tag_data_offset<s_tags_header>(cache_files_get_header()->tag_offset_mask);
}

cache_file_tag_instance* global_tag_instances_get(void)
{
	return *Memory::GetAddress<cache_file_tag_instance**>(0x47CD50, 0x4A29B8);
}

tag_iterator* tag_iterator_new(tag_iterator* itr, blam_tag::tag_group_type type)
{
	itr->next_tag_index = 0;
	itr->tag_type.tag_type = type;
	return itr;
}

datum __cdecl tag_iterator_next(tag_iterator* itr)
{
	return INVOKE(0x3172C, 0x255DC, tag_iterator_next, itr);
}

void cache_file_map_clear_all_failures(void)
{
	typedef void(__cdecl *cache_file_map_clear_all_failures_t)(void);
	auto p_cache_file_map_clear_all_failures = Memory::GetAddress<cache_file_map_clear_all_failures_t>(0x64551, 0x4C5A8);
	p_cache_file_map_clear_all_failures();
	return;
}

void* __cdecl tag_get_fast(datum tag_index)
{
	return INVOKE(0x239623, 0x217295, tag_get_fast, tag_index);
}