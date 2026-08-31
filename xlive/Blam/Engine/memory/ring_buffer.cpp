#include "stdafx.h"
#include "ring_buffer.h"

/* public code */

int32 c_ring_buffer::size(void) const
{
	return ring_size;
}

int32 c_ring_buffer::count(void) const
{
	ASSERT(element_count>=0 && element_count<=ring_size);

	return element_count;
}

bool c_ring_buffer::empty(void) const
{
	ASSERT(element_count>=0 && element_count<=ring_size);
	
	return element_count == 0;
}

bool c_ring_buffer::full(void) const
{
	ASSERT(element_count>=0 && element_count<=ring_size);

	return element_count == ring_size;
}


void c_ring_stream::attach(
	int32 storage_size,
	uint8* storage_buffer)
{
	INVOKE_TYPE(0x3814EF, 0x32CC29, void(__thiscall*)(c_ring_stream*, int32, void*), this, storage_size, storage_buffer);
	return;
}

void c_ring_stream::detach(void)
{
	storage = nullptr;
	ring_size = NULL;
	return;
}

bool c_ring_stream::attached(void) const
{
	return storage != nullptr;
}

int32 c_ring_stream::add_block(
	int32 block_size,
	void* block_data)
{
	return INVOKE_TYPE(0x381507, 0x32CC41, uint32(__thiscall*)(c_ring_stream*, int32, void*), this, block_size, block_data);
}

void c_ring_stream::remove_block(
	int32 block_size,
	void* block_buffer)
{
	return INVOKE_TYPE(0x381546, 0x322C80, void(__thiscall*)(c_ring_stream*, int32, void*), this, block_size, block_buffer);
}
