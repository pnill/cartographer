#include "stdafx.h"
#include "TagInterface.h"

tags::tag_instance* tags::get_tag_instances()
{
	return *Memory::GetAddress<tag_instance**>(0x47CD50, 0x4A29B8);
}

char* tags::get_tag_data()
{
	return *Memory::GetAddress<char**>(0x47CD54, 0x4A29BC);
}

HANDLE tags::get_cache_handle()
{
	return *Memory::GetAddress<HANDLE*>(0x4AE8A8, 0x4CF128);
}

bool tags::cache_file_loaded()
{
	return *Memory::GetAddress<bool*>(0x47CD60, 0x4A29C8);
}

static std::vector<void(*)()> load_callbacks;
void tags::on_map_load(void(*callback)())
{
	load_callbacks.push_back(callback);
}

void __cdecl tags::run_callbacks()
{
	// load debug names before any callbacks are called
	for (auto callback : load_callbacks)
		callback();
}