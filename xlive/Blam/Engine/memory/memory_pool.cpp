#include "stdafx.h"
#include "memory_pool.h"

bool memory_pool_block_allocate_handle(s_memory_pool* memory_pool, void* data, int data_size, int a4, int a5)
{
	typedef bool(__cdecl* memory_pool_block_allocate_handle_t)(s_memory_pool* memory_pool, void* data, int data_size, int a4, int a5);
	auto p_memory_pool_block_allocate_handle = Memory::GetAddress<memory_pool_block_allocate_handle_t>(0x8C08E, 0x81C32);

	return p_memory_pool_block_allocate_handle(memory_pool, data, data_size, a4, a5);
}

bool memory_pool_block_reallocate_handle(s_memory_pool* memory_pool, void* data, void* a3, const char* name, void* a5)
{
	typedef bool(__cdecl* memory_pool_block_reallocate_handle_t)(s_memory_pool* memory_pool, void* data, void* a3, const char* name, void* a5);
	auto p_memory_pool_block_reallocate_handle = Memory::GetAddress<memory_pool_block_reallocate_handle_t>(0x8C12C, 0x81CD0);

	return p_memory_pool_block_reallocate_handle(memory_pool, data, a3, name, a5);
}
