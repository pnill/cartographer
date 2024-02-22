#include "stdafx.h"
#include "tag_groups.h"

tag_group tag_group_get_name(tag_group group)
{
	// We need to reverse this since little endian reorders the character bytes
	tag_group reversed_group;
	reversed_group.string[0] = group.string[3];
	reversed_group.string[1] = group.string[2];
	reversed_group.string[2] = group.string[1];
	reversed_group.string[3] = group.string[0];
	return reversed_group;
}

void* __cdecl tag_block_get_element_with_size(const s_tag_block* block, int32 index, int32 block_size)
{
	return INVOKE(0x3C772, 0x32899, tag_block_get_element_with_size, block, index, block_size);
}
