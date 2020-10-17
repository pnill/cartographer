#pragma once
#include "Blam/Cache/DataTypes/DatumIndex.h"

namespace MetaExtender
{
	void add_tag_block(unsigned long tag_block_ptr, int tag_block_item_size, int tag_block_item_count, int newCount);
	void test();
	void free_tag_blocks();
}
