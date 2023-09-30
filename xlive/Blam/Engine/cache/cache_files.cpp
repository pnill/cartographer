#include "stdafx.h"
#include "cache_files.h"

#include "Blam/Engine/game/game_globals.h"

cache_file_tag_instance* global_tag_instances_get(void)
{
	return *Memory::GetAddress<cache_file_tag_instance**>(0x47CD50, 0x4A29B8);
}

datum s_tag_data_iterator::get_next_datum()
{
	typedef datum(__cdecl t_get_next_datum)(s_tag_data_iterator* it);
	auto p_get_next_datum = Memory::GetAddress<t_get_next_datum*>(0x3172C);
	return  p_get_next_datum(this);
}

void cache_file_map_clear_all_failures(void)
{
	typedef void(__cdecl *cache_file_map_clear_all_failures_t)(void);
	auto p_cache_file_map_clear_all_failures = Memory::GetAddress<cache_file_map_clear_all_failures_t>(0x64551, 0x4C5A8);
	p_cache_file_map_clear_all_failures();
	return;
}
