#include "stdafx.h"
#include "data.h"

#include "cseries/runtime_state.h"
#include "rockall_heap_manager.h"

/* prototypes */

static int32 data_next_absolute_index(data_array* data, int32 index);

/* macros */

#define DATA_HEADER_GET(data, index) \
((s_datum_header*)(((char*)assert_return((data))->data) + (assert_return((data))->size * DATUM_INDEX_TO_ABSOLUTE_INDEX(index))))

/* public code */

void* datum_get(const data_array* data, int32 index)
{
	s_datum_header* header = DATA_HEADER_GET(data, index);

	ASSERT(data);
	ASSERT(data->valid);
	vassert(index != NONE, "tried to access %s index NONE", data->name);
	vassert(
		DATUM_INDEX_TO_IDENTIFIER(index) != 0,
		"tried to access %s using datum_get() with an absolute index # % d",
		data->name,
		index
	);
	vassert(
		DATUM_INDEX_TO_ABSOLUTE_INDEX(index) < data->first_free_absolute_index,
		"%s index #%d (0x%x) is out of range (%d)",
		data->name,
		DATUM_INDEX_TO_ABSOLUTE_INDEX(index),
		index,
		data->first_free_absolute_index
	);
	vassert(
		header->identifier,
		"%s index #%d (0x%x) is unused",
		data->name,
		DATUM_INDEX_TO_ABSOLUTE_INDEX(index),
		index
	);
	vassert(
		header->identifier == DATUM_INDEX_TO_IDENTIFIER(index),
		"%s index #%d (0x%x) is changed, should be 0x%x",
		data->name,
		DATUM_INDEX_TO_ABSOLUTE_INDEX(index),
		index,
		DATUM_INDEX_NEW(DATUM_INDEX_TO_ABSOLUTE_INDEX(index), header->identifier)
	);
	ASSERT(header == align_pointer(header, data->alignment_bits));

	return header;
}

void* datum_try_and_get(const data_array* data, int32 index)
{
	void* result = NULL;
	ASSERT(data);
	ASSERT(data->valid);

	if (index != NONE)
	{
		vassert(
			DATUM_INDEX_TO_IDENTIFIER(index) != 0,
			"tried to access %s using datum_try_and_get() with an absolute index #%d",
			data->name,
			index
		);
		vassert(
			DATUM_INDEX_TO_ABSOLUTE_INDEX(index) < data->maximum_count,
			"tried to access %s using datum_try_and_get() with an index 0x%08X outside maximum range [0, %d)",
			data->name,
			index,
			data->maximum_count
		);

		if (DATUM_INDEX_TO_ABSOLUTE_INDEX(index) < data->first_free_absolute_index)
		{
			s_datum_header* header = DATA_HEADER_GET(data, index);
			if (header->identifier)
			{
				if (header->identifier == DATUM_INDEX_TO_IDENTIFIER(index))
				{
					result = header;
				}
			}
		}
		else
		{
			result = NULL;
		}
	}

	ASSERT(result == align_pointer(result, data->alignment_bits));

	return result;
}

void* datum_get_absolute(const data_array* data, int32 index)
{
	s_datum_header* header = DATA_HEADER_GET(data, index);

	ASSERT(data);
	ASSERT(data->valid);
	vassert(index != NONE, "tried to access %s index NONE", data->name);
	vassert(DATUM_INDEX_TO_IDENTIFIER(index) == 0, "tried to access %s using datum_get_absolute() with a non absolute index #%d", data->name, index);
	vassert(VALID_INDEX(index, data->first_free_absolute_index), "%s absolute index #%d is out of range (%d)", data->name, index, data->first_free_absolute_index);
	vassert(header->identifier, "%s absolute index #%d is unused", data->name, index);
	ASSERT(header == align_pointer(header, data->alignment_bits));

	return header;
}

void* datum_try_and_get_absolute(const data_array* data, int32 index)
{
	s_datum_header* result = NULL;

	ASSERT(data);
	ASSERT(data->valid);
	vassert(index != NONE, "tried to access %s index NONE", data->name);
	vassert(DATUM_INDEX_TO_IDENTIFIER(index) == 0, "tried to access %s using datum_try_and_get_absolute() with a non absolute index #%d", data->name, index);
	vassert(VALID_INDEX(index, data->first_free_absolute_index), "%s absolute index #%d is out of range (%d)", data->name, index, data->first_free_absolute_index);

	if (DATUM_INDEX_TO_ABSOLUTE_INDEX(index) < data->first_free_absolute_index)
	{
		s_datum_header* header = DATA_HEADER_GET(data, index);
		if (header->identifier)
		{
			if (header->identifier == DATUM_INDEX_TO_IDENTIFIER(index))
			{
				result = header;
			}
		}
	}
	else
	{
		result = NULL;
	}

	return result;
}

void __cdecl datum_delete(data_array* data, datum datum_index)
{
	INVOKE(0x6693E, 0x3262A, datum_delete, data, datum_index);
	return;
}

int32 data_allocation_size(int32 maximum_count, int32 size, int32 alignment_bits)
{
	ASSERT(maximum_count > 0 && maximum_count <= k_unsigned_short_max);
	ASSERT(size > 0);
	ASSERT(alignment_bits >= 0);
	ASSERT((size_t)size == align_address(size, alignment_bits));

	const int32 alignment = (1 << alignment_bits);
	return size * maximum_count + BIT_VECTOR_SIZE_IN_BYTES(maximum_count) + alignment + 75;
}

void __cdecl data_initialize(
	data_array* data,
	const char* data_name,
	int32 maximum_count,
	int32 size,
	int32 alignment_bits,
	c_allocation_base* allocator)
{
	return INVOKE(0x665AD, 0x32299, data_initialize, data, data_name, maximum_count, size, alignment_bits, allocator);
}

#ifdef _DEBUG
data_array* data_new(
	const char* data_name,
	int32 maximum_count,
	int32 size,
	int32 alignment_bits,
	const char* filename,
	int32 line_number,
	c_allocation_base* allocator)
#else
data_array* data_new(
	const char* data_name,
	int32 maximum_count,
	int32 size,
	int32 alignment_bits,
	c_allocation_base* allocator)
#endif
{
	int32 alloc_size = data_allocation_size(maximum_count, size, alignment_bits);
	data_array* result = (data_array*)(allocator)->alloc(alloc_size);

	if (result)
	{
		data_initialize(result, data_name, maximum_count, size, alignment_bits, allocator);
		SET_BIT(result->flags, _data_array_protection_bit, true);
	}

	return result;
}


#ifdef _DEBUG
void data_dispose(data_array* data, const char* filename, int32 line_number)
#else
void data_dispose(data_array* data)
#endif
{
	csmemset(data, 0, sizeof(data_array));
	if (data->allocator)
	{
		data->allocator->free_block(data);
	}
	return;
}

void __cdecl data_connect(data_array* data, int32 size, void* data_pointer)
{
	INVOKE(0x66633, 0x3231F, data_connect, data, size, data_pointer);
	return;
}

void data_disconnect(data_array* data)
{
	ASSERT(!TEST_BIT(data->flags, _data_array_disconnected_bit));
	ASSERT(SET_BIT(data->flags, _data_array_can_disconnect_bit, true));

	SET_BIT(data->flags, _data_array_disconnected_bit, true);
	data->data = NULL;
	data->valid = false;
	return;
}

void __cdecl data_delete_all(data_array* data)
{
	INVOKE(0x66715, 0x32401, data_delete_all, data);
	return;
}

datum __cdecl datum_new(data_array* data)
{
	ASSERT(data);
	ASSERT(!TEST_BIT(data->flags, _data_array_disconnected_bit));
	ASSERT(data->data != NULL);
	
	data_verify(data);
	ASSERT(data->valid);

	return INVOKE(0x667A0, 0x3248C, datum_new, data);
}

datum __cdecl datum_new_at_index(data_array* data_array, datum datum_index)
{
	return INVOKE(0x66858, 0x32544, datum_new_at_index, data_array, datum_index);
}

datum __cdecl datum_new_at_absolute_index(data_array* data, int32 absolute_index)
{
	ASSERT(DATUM_INDEX_TO_ABSOLUTE_INDEX(absolute_index) == absolute_index);
	ASSERT(DATUM_INDEX_TO_IDENTIFIER(absolute_index) == 0);

	return INVOKE(0x668D5, 0x325C1, datum_new_at_absolute_index, data, absolute_index);
}


uint32 __cdecl datum_header_allocate(uint32 total_size, uint32 alignment_bits)
{
	return INVOKE(0x37E69, 0x2B4E6, datum_header_allocate, total_size, alignment_bits);
}

bool __cdecl datum_header_deallocate(void* object)
{
	return INVOKE(0x37EC3, 0x2B540, datum_header_deallocate, object);
}

void data_make_valid(data_array* data)
{
	ASSERT(data);

	data->valid = true;
	data_delete_all(data);
	data_verify(data);
	return;
}

int32 data_next_index(data_array* data, datum index)
{
	index = (index == NONE ? 0 : DATUM_INDEX_TO_ABSOLUTE_INDEX(index) + 1);
	const int32 absolute_index = data_next_absolute_index(data, index);
	return datum_absolute_index_to_index(data, absolute_index);
}

void data_make_invalid(data_array* data)
{
	data_verify(data);
	data->valid = false;
	return;
}

datum __cdecl datum_absolute_index_to_index(data_array* data, int32 absolute_index)
{
	return INVOKE(0x664C3, 0x321AF, datum_absolute_index_to_index, data, absolute_index);
}

void data_verify(const data_array* data)
{
	ASSERT(data);
	vassert(
		data->signature == k_data_array_signature &&
		data->maximum_count >= 0 &&
		IN_RANGE(data->first_free_absolute_index, 0, data->maximum_count) &&
		IN_RANGE(data->bit_index_size, 0, data->maximum_count) &&
		IN_RANGE(data->actual_count, 0, data->first_free_absolute_index) &&
		(TEST_BIT(data->flags, _data_array_disconnected_bit) || data->data) &&
		data->in_use_bit_vector,
		"%s data array @%p is bad or not allocated",
		data->name,
		data
	);
	return;
}

void iterator_new(data_iterator* iterator, data_array* data)
{
	data_verify(data);
	ASSERT(data->valid);

	iterator->data = data;
	iterator->absolute_index = NONE;
	iterator->index = NONE;
	return;
}

void* iterator_next(data_iterator* iterator)
{
	const data_array* data = iterator->data;
	data_verify(data);
	ASSERT(data->valid);

	const int32 absolute_index = data_next_absolute_index(iterator->data, iterator->absolute_index + 1);

	s_datum_header* result;
	if (absolute_index == NONE)
	{
		result = NULL;
		iterator->absolute_index = data->maximum_count;
		iterator->index = NONE;
	}
	else
	{
		result = DATA_HEADER_GET(data, absolute_index);
		iterator->absolute_index = absolute_index;
		iterator->index = DATUM_INDEX_NEW(absolute_index, result->identifier);

		ASSERT(iterator->index != NONE);
		ASSERT(((result == NULL) && (iterator->index == NONE)) || ((result != NULL) && (iterator->index != NONE)));
	}
	return result;
}

/* private code */

static int32 data_next_absolute_index(data_array* data, int32 index)
{
	if (index < 0 || index >= data->first_free_absolute_index)
	{
		index = NONE;
	}
	else
	{
		while (!BIT_VECTOR_TEST_FLAG(data->in_use_bit_vector, index))
		{
			if (++index >= data->first_free_absolute_index)
			{
				index = NONE;
				break;
			}
		}
	}

	return index;
}
