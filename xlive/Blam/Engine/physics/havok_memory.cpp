#include "stdafx.h"
#include "havok_memory.h"

/* structures */

struct s_havok_memory_globals
{
	bool initialized;
	int32 last_cleanup_time;
	char* memory_allocator_buffer;
	void* second_overflow_pool_buffer;
	e_memory_allocator_type active_memory_allocator_type;
	class hkMemory* startup_pool_memory;
	class hkMemory* runtime_pool_memory;
	class hkMemory* overflow_pool_memory;
	class hkMemory* second_overflow_pool_memory;
};

/* prototypes */

static s_havok_memory_globals* havok_memory_globals_get(void);

static bool* region_memory_being_borrowed_get(void);

/* public code */

bool havok_memory_allocator_locked(void)
{
	return INVOKE(0xF78F7, 0xDE732, havok_memory_allocator_locked);
}

bool is_havok_update_memory_initialized(void)
{
	return *region_memory_being_borrowed_get() == false;
}

void __cdecl havok_memory_garbage_collect(void)
{
	INVOKE(0xF7F78, 0xDEDB3, havok_memory_garbage_collect);
	return;
}

/* private code */

static s_havok_memory_globals* havok_memory_globals_get(void)
{
	return Memory::GetAddress<s_havok_memory_globals*>(0x41D4F4, 0x0);
}


static bool* region_memory_being_borrowed_get(void)
{
	return Memory::GetAddress<bool*>(0x4D2D5C, 0x4F6C14);
}
