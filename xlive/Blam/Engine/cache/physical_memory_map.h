#pragma once

class c_physical_memory
{
public:
	static void* allocate(uint32 size);
	static bool deallocate(void* buffer);
};