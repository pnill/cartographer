#include "stdafx.h"
#include "memory_pool.h"

bool __cdecl memory_pool_block_allocate_handle(s_memory_pool* memory_pool, void* data, int data_size, int a4, int a5)
{
	typedef bool(__cdecl* memory_pool_block_allocate_handle_t)(s_memory_pool* memory_pool, void* data, int data_size, int a4, int a5);
	auto p_memory_pool_block_allocate_handle = Memory::GetAddress<memory_pool_block_allocate_handle_t>(0x8C08E, 0x81CD0);

	return p_memory_pool_block_allocate_handle(memory_pool, data, data_size, a4, a5);
}
