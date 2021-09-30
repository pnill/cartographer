#pragma once

#include "Blam\Cache\DataTypes\BlamDataTypes.h"

// The game is using some sort of heap manager developed by Microsoft in 2000's named RockAll Heap Manager 

struct s_data_array
{
	char name[0x20];				// 0x0
	int datum_max_elements;			// 0x20
	int datum_element_size;			// 0x24
	BYTE alignment_bit;				// 0x28
	bool is_valid;					// 0x29
	WORD flags;						// 0x2A
	char data_signature[4];			// 0x2C
	void **allocator;				// 0x30
	struct {
		int bit_index_size;			// 0x34
		int max_data_count;			// 0x38
	} active_indices; 
	int total_elements_used;		// 0x3C 
	datum next_datum;				// 0x40
	char* data;					// 0x44
	int* data_usable_bit_mask;		// 0x48
};
CHECK_STRUCT_SIZE(s_data_array, 0x4C);

template<typename T = void*>
class s_data_iterator
{
public:

	s_data_iterator(s_data_array* _data_array) : 
		m_data_array(_data_array),
		m_current_absolute_index(NONE),
		m_last_datum_index(DATUM_NONE)
	{
	}

	~s_data_iterator()
	{
	}

	s_data_array* get_datum_array()
	{
		return m_data_array;
	}

	T* get_data_at_datum_index(datum datum_index)
	{
		return reinterpret_cast<T*>(&m_data_array->data[m_data_array->datum_element_size * DATUM_ABSOLUTE_INDEX(datum_index)]);
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
			m_last_datum_index = DATUM_NONE;
			m_current_absolute_index = m_data_array->datum_max_elements;
		}
		else
		{
			result = reinterpret_cast<T*>(&m_data_array->data[m_data_array->datum_element_size * index]);
			m_current_absolute_index = index;
			m_last_datum_index = DATUM_NEW(index, *(unsigned short*)(result)); // absolute index w/ salt
		}
		return result;
	}
	
	int get_next_absolute_datum_index(int index)
	{
		if (index < 0)
			return -1;

		if (index >= m_data_array->active_indices.max_data_count)
			return -1;

		while (!((1 << (index & 0x1F)) & m_data_array->data_usable_bit_mask[index >> 5]))
		{
			if (++index >= m_data_array->active_indices.max_data_count)
				return -1;
		}
		return index;
	}

	int get_current_absolute_index()
	{
		return m_current_absolute_index;
	}

	datum get_current_datum_index()
	{
		return m_last_datum_index;
	}

private:
	
	s_data_array* m_data_array;
	datum m_last_datum_index;
	int m_current_absolute_index;
};
