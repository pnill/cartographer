#pragma once

struct s_memory_pool
{
	uint32 signature;
	char name[32];
	void* allocator;
	uint32 size;
	uint32 free_size;
	uint32 offset_to_data;
	uint32 first_block_handle;
	uint32 last_block_handle;
	bool reference_tracking;
	bool allocation_from_anywhere_in_pool;
	bool verification_enabled;
	bool unk_bool_3F;
};

int32 __cdecl memory_pool_block_free(s_memory_pool* memory_pool, void** payload_data);