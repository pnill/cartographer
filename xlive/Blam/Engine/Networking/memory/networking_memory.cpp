#include "stdafx.h"

#include "networking_memory.h"

uint8* network_heap_allocate_block(uint32 size)
{
	return INVOKE(0x1AC939, 0x1ACB07, network_heap_allocate_block, size);
}

void network_heap_free_block(uint8* block)
{
	return INVOKE(0x1AC94A, 0x1D9B6E, network_heap_free_block, block);
}