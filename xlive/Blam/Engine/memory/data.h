#pragma once

/* constants */

enum
{
	k_data_array_signature = 'd@t@',
	k_data_iterator_signature = 'iter',
};

/* enums */

enum
{
	_data_array_can_disconnect_bit = 0,
	_data_array_disconnected_bit,
	_data_array_protection_bit
};

/* structures */

struct s_datum_header
{
	uint16 identifier;
};

struct data_array
{
	char name[32];
	int32 maximum_count;
	int32 size;
	int8 alignment_bits;
	bool valid;
	int16 flags;
	int32 signature;
	class c_allocation_base* allocator;
	int32 bit_index_size;
	int32 first_free_absolute_index;
	int32 actual_count;
	int32 count;
	void* data;
	int32* in_use_bit_vector;
};
ASSERT_STRUCT_SIZE(data_array, 76);

struct data_iterator
{
	data_array* data;
	datum index;
	int32 absolute_index;

	// This is only included in debug builds
	//uint32 signature;
};
ASSERT_STRUCT_SIZE(data_iterator, 12);

/* prototypes */

// Get data in data_array from datum index
void* datum_get(const data_array* data, int32 index);

void* datum_try_and_get(const data_array* data, int32 datum_index);

void* datum_get_absolute(const data_array* data, int32 index);

void* datum_try_and_get_absolute(const data_array* data, int32 datum_index);

void __cdecl datum_delete(data_array* data, datum datum_index);

int32 data_allocation_size(int32 maximum_count, int32 size, int32 alignment_bits);

void __cdecl data_initialize(
	data_array* data,
	const char* data_name,
	int32 maximum_count,
	int32 size,
	int32 alignment_bits,
	class c_allocation_base* allocator);

// Parameters differ between release and debug
// We include the file and line number in debug and not release
#ifdef _DEBUG
data_array* data_new(
	const char* data_name,
	int32 maximum_count,
	int32 size,
	int32 alignment_bits,
	const char* filename,
	int32 line_number,
	class c_allocation_base* allocator);
#define DATA_NEW(data_name, maximum_count, size, alignment_bits, allocator) \
data_new(data_name, maximum_count, size, alignment_bits, __FILE__, __LINE__, allocator);
#else
data_array* data_new(
	const char* data_name,
	int32 maximum_count,
	int32 size,
	int32 alignment_bits,
	c_allocation_base* allocator);
#define DATA_NEW(data_name, maximum_count, size, alignment_bits, allocator) \
data_new(data_name, maximum_count, size, alignment_bits, allocator);
#endif

#ifdef _DEBUG
void data_dispose(data_array* data, const char* filename, int32 line_number);

#define DATA_DISPOSE(data_name) data_dispose(data_name, __FILE__, __LINE__);
#else

void data_dispose(data_array* data);

#define DATA_DISPOSE(data_name) data_dispose(data_name);
#endif

void __cdecl data_connect(data_array* data, int32 size, void* data_pointer);

void data_disconnect(data_array* data);

void __cdecl data_delete_all(data_array* data);

datum __cdecl datum_new(data_array* data);

datum __cdecl datum_new_at_index(data_array* data_array, datum datum_index);

datum __cdecl datum_new_at_absolute_index(data_array* data, int32 absolute_index);

uint32 __cdecl datum_header_allocate(uint32 total_size, uint32 alignment_bits);

bool __cdecl datum_header_deallocate(void* object);

void data_make_valid(data_array* data);

int32 data_next_index(data_array* data, datum index);

void data_make_invalid(data_array* data);

datum __cdecl datum_absolute_index_to_index(data_array* data, int32 absolute_index);

void data_verify(const data_array* data);

void iterator_new(data_iterator* iterator, data_array* data);

void* __cdecl iterator_next(data_iterator* iterator);
