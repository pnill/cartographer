#pragma once

#include "BlamPrimitiveType.h"

/*********************************************************************
* data_block
* 8 BYTE Tag Structure for any Data Block Field
**********************************************************************/

struct data_block
{
	data_block(uint32_t BlockSize) :
		block_size(BlockSize)
	{
	}

	data_block(uint32_t _block_size, uint32_t _block_offset) :
		block_size(_block_size),
		block_offset(_block_offset)
	{
	}
	//Holds size of the Tag Data Block field
	uint32_t block_size = 0;
	//Offset from Tag Data start
	uint32_t block_offset = 0;

};
CHECK_STRUCT_SIZE(data_block, 8);