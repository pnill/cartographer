#include "stdafx.h"
#include "physical_memory_map.h"
#include "memory/data.h"

void* c_physical_memory::allocate(uint32 size)
{
	return (void*)datum_header_allocate(size, 12);
}

bool c_physical_memory::deallocate(void* buffer)
{
	return datum_header_deallocate(buffer);
}
