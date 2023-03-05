#pragma once

struct s_memory_pool
{
	DWORD signature;			// Character literal
	byte gap[28];
	DWORD size_function;
	DWORD unk2C;
	DWORD size;
	DWORD free_size;
	DWORD unk30;
	DWORD unk34;
	DWORD last_block_handle;
	bool unk_bool3C;
	bool unk_bool3D;
	bool unk_bool3E;
	bool unk_bool3F;
};

bool memory_pool_block_allocate_handle(s_memory_pool* a1, void* data, int data_size, int a4, int a5);
bool memory_pool_block_reallocate_handle(s_memory_pool* memory_pool, void* data, void* a3, const char* name, void* a5);