#include "stdafx.h"
#include "TagInterface.h"

tags::tag_instance* tags::get_tag_instance(datum tag_index)
{
	tags::tag_instance* tag_instances = *Memory::GetAddress<tag_instance**>(0x47CD50, 0x4A29B8);
	return &tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_index)];
}

uint8* tags::get_tag_data()
{
	return *Memory::GetAddress<uint8**>(0x47CD54, 0x4A29BC);
}

HANDLE tags::get_cache_handle()
{
	return *Memory::GetAddress<HANDLE*>(0x4AE8A8, 0x4CF128);
}

bool tags::cache_file_loaded()
{
	return *Memory::GetAddress<bool*>(0x47CD60, 0x4A29C8);
}
