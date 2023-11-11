#include "stdafx.h"
#include "memory_pool.h"

int32 __cdecl memory_pool_block_free(s_memory_pool* memory_pool, void** payload_data)
{
	return INVOKE(0x8BD80, 0x81924, memory_pool_block_free, memory_pool, payload_data);
}