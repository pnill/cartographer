#include "stdafx.h"

#include "networking_memory.h"



static s_network_heap_stats g_network_heap_allocations;

c_network_heap* network_get_heap()
{
	return *Memory::GetAddress<c_network_heap**>(0x4FADE0, 0x525298);
}

s_network_heap_stats* network_heap_get_description()
{
	return &g_network_heap_allocations;
}

int32 c_network_heap::get_block_size(const uint8* block) const
{
	int32 size;
	if (!rockall_frontend->Details(block, &size))
	{
		size = -1;
	}

	return size;
}

typedef uint8* (__cdecl* t_network_heap_allocate_block)(uint32 size);
t_network_heap_allocate_block p_network_heap_allocate_block;

uint8* __cdecl network_heap_allocate_block(uint32 size)
{
	//uint8* block = INVOKE(0x1AC939, 0x1ACB07, network_heap_allocate_block, size);
	uint8* block = p_network_heap_allocate_block(size);

	if (block)
	{
		g_network_heap_allocations.allocations++;
		g_network_heap_allocations.allocations_in_bytes += network_get_heap()->get_block_size(block);
	}

	return block;
}

typedef void(__cdecl* t_network_heap_free_block)(uint8* block);
t_network_heap_free_block p_network_heap_free_block;

void __cdecl network_heap_free_block(uint8* block)
{
	int32 block_size = network_get_heap()->get_block_size(block);

	if (block_size > 0)
	{
		g_network_heap_allocations.allocations--;
		g_network_heap_allocations.allocations_in_bytes -= block_size;
	}

	return p_network_heap_free_block(block);
	// return INVOKE(0x1AC94A, 0x1ACB18, network_heap_free_block, block);
}

void c_network_heap::dispose()
{
	g_network_heap_allocations.allocations = 0;
	g_network_heap_allocations.allocations_in_bytes = 0;
	return INVOKE_TYPE(0x381574, 0x32CCAE, void(__thiscall*)(c_network_heap*), this);
}

__declspec(naked) void jmp_c_network_heap__discard() { __asm { jmp c_network_heap::dispose } }

void network_memory_apply_patches(void)
{
	// hook the heap allocator globally, to get a picture of the network heap usage
	DETOUR_ATTACH(p_network_heap_allocate_block, Memory::GetAddress<t_network_heap_allocate_block>(0x1AC939, 0x1ACB07), network_heap_allocate_block);
	DETOUR_ATTACH(p_network_heap_free_block, Memory::GetAddress<t_network_heap_free_block>(0x1AC94A, 0x1ACB18), network_heap_free_block);
	PatchCall(Memory::GetAddress(0x1AD292, 0x1AD460), jmp_c_network_heap__discard);
	return;
}