#pragma once

// The game is using some sort of heap manager developed by Microsoft in 2000's named RockAll Heap Manager 
struct s_data_array
{
	char name[32];				// 0x0
	int32 datum_max_elements;			// 0x20
	int32 datum_element_size;			// 0x24
	int8 alignment_bits;				// 0x28
	bool is_valid;					// 0x29
	int16 flags;						// 0x2A
	int32 data_signature;			// 0x2C
	void **allocator;				// 0x30
	int bit_index_size;				// 0x34
	int next_unused_index;					// 0x38
	int total_elements_used;		// 0x3C 
	int field_40;					// 0x40
	char* data;						// 0x44
	int32 *in_use_bit_vector;		// 0x48

};
ASSERT_STRUCT_SIZE(s_data_array, 0x4C);

template<typename T = void*>
class s_data_iterator
{
public:
	s_data_iterator(s_data_array* _data_array) : 
		m_data_array(_data_array),
		m_current_absolute_index(NONE),
		m_last_datum_index(NONE)
	{
	}

	s_data_array* get_datum_array() const
	{
		return m_data_array;
	}

	T* get_data_at_datum_index(datum datum_index) const
	{
		return reinterpret_cast<T*>(&m_data_array->data[m_data_array->datum_element_size * DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index)]);
	};

	T* get_current_datum()
	{
		return reinterpret_cast<T*>(&m_data_array->data[m_data_array->datum_element_size * m_current_absolute_index]);
	}

	T* get_next_datum()
	{
		T* result = nullptr;
		int index = get_next_absolute_datum_index(m_current_absolute_index + 1);

		if (index == -1)
		{
			result = nullptr;
			m_last_datum_index = NONE;
			m_current_absolute_index = m_data_array->datum_max_elements;
		}
		else
		{
			result = reinterpret_cast<T*>(&m_data_array->data[m_data_array->datum_element_size * index]);
			m_current_absolute_index = index;
			m_last_datum_index = DATUM_INDEX_NEW(index, *(int16*)(result)); // absolute index w/ salt
		}
		return result;
	}
	
	int32 get_next_absolute_datum_index(int index) const
	{
		if (index < 0)
			return -1;

		if (index >= m_data_array->next_unused_index)
			return -1;

		while (!BIT_VECTOR_TEST_FLAG(m_data_array->in_use_bit_vector, index))
		{
			if (++index >= m_data_array->next_unused_index)
				return -1;
		}
		return index;
	}

	int32 get_current_absolute_index() const
	{
		return m_current_absolute_index;
	}

	datum get_current_datum_index() const
	{
		return m_last_datum_index;
	}

	int32 get_data_count() const
	{
		return m_data_array->total_elements_used;
	}

	void reset()
	{
		m_last_datum_index = NONE;
		m_current_absolute_index = NONE;
	}
private:
	s_data_array* m_data_array;
	datum m_last_datum_index;
	int32 m_current_absolute_index;
};


// Get data in s_data_array from datum index
void* datum_get(const s_data_array* data, datum datum_index);
void* datum_try_and_get(const s_data_array* data_array, datum datum_index);
datum __cdecl datum_new(s_data_array* data_array);

void __cdecl data_make_valid(s_data_array* data_array);
void __cdecl datum_delete(s_data_array* data_array, datum datum_index);

uint32 datum_header_allocate(uint32 total_size, uint32 alignment_bits);
bool __cdecl datum_header_deallocate(void* object);