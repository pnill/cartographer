#include "stdafx.h"
#include "cseries_system_memory.h"

/* public code */

void* system_heap_alloc(size_t size)
{
	return HeapAlloc(GetProcessHeap(), 0, size);
}

BOOL system_heap_free(void* block)
{
	return HeapFree(GetProcessHeap(), 0, block);
}
