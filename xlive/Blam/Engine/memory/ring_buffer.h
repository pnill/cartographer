#pragma once

/* classes */

class c_ring_buffer
{
public:
	int32 size(void) const;
	int32 count(void) const;
	bool empty(void) const;
	bool full(void) const;

protected:
	int32 ring_size;
	uint8* storage;
	int32 element_count;
	int32 data_size;
};
ASSERT_STRUCT_SIZE(c_ring_buffer, 16);


class c_ring_stream : c_ring_buffer
{
public:
	void attach(int32 storage_size, uint8* storage_buffer);
	void detach(void);

	bool attached(void) const;

	int32 add_block(int32 block_size, void* block_data);
	void remove_block(int32 block_size, void* block_buffer);

private:
	// bool uses_debug_signature; debug builds only
};
ASSERT_STRUCT_SIZE(c_ring_stream, 16);
