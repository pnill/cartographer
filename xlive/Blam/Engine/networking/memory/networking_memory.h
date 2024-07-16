#pragma once

#include "memory/rockall_heap_manager.h"

struct s_network_heap_stats
{
	int32 allocations;
	int32 allocations_in_bytes;
};

class c_network_heap
{
public:
	c_fixed_memory_rockall_frontend* rockall_frontend;
	int32 get_block_size(const uint8* block) const;

	void dispose();
};

uint8* __cdecl network_heap_allocate_block(uint32 size);

void __cdecl network_heap_free_block(uint8* block);

void network_heap_verify_block(uint8* block);

s_network_heap_stats* network_heap_get_description();

void network_memory_apply_patches();
