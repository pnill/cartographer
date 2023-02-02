#pragma once

struct s_memory_pool
{
	byte gap[64];
};

bool __cdecl memory_pool_block_allocate_handle(s_memory_pool* a1, void* data, int data_size, int a4, int a5);
