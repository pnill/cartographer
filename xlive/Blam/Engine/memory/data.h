#pragma once

#include "Blam/Cache/DataTypes/BlamDataTypes.h"

template<typename T = int>
struct s_bitflags
{
	T* m_flags;

	// only power of 2 sized types
	static_assert(sizeof(T) > 0 && (sizeof(T) & (sizeof(T) - 1)) == 0);

	bool test_bit(int index) const
	{
		return (m_flags[index / (CHAR_BIT * sizeof(T))] & FLAG(index & (CHAR_BIT * sizeof(T) - 1))) != 0;
	}

	void set_bit(int index, bool state)
	{
		if (state)
			m_flags[index / (CHAR_BIT * sizeof(T))] |= m_flags[index / (CHAR_BIT * sizeof(T))] & FLAG(index & (CHAR_BIT * sizeof(T) - 1));
		else
			m_flags[index / (CHAR_BIT * sizeof(T))] = m_flags[index / (CHAR_BIT * sizeof(T))] & ~(FLAG(index & (CHAR_BIT * sizeof(T) - 1)));
	}
};

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
	int bit_index_size;				// 0x34
	int next_unused_index;					// 0x38
	int total_elements_used;		// 0x3C 
	int field_40;					// 0x40
	char* data;						// 0x44
	s_bitflags<> active_bit_mask;		// 0x48

	static datum datum_new_in_range(s_data_array* data_array)
	{
		auto p_datum_new_in_range = Memory::GetAddress<datum(__cdecl*)(s_data_array*)>(0x667A0, 0x3248C);
		return p_datum_new_in_range(data_array);
	}

	static void data_make_valid(s_data_array* data_array)
	{
		// not entirely sure what this actually does
		auto p_data_make_valid = Memory::GetAddress<void(_cdecl*)(s_data_array*)>(0x66B33, 0x0); // TODO DEDI OFFSET
		return p_data_make_valid(data_array);
	}
};
CHECK_STRUCT_SIZE(s_data_array, 0x4C);

template<typename T = void*>
class s_data_iterator
{
public:

	s_data_iterator(s_data_array* _data_array) : 
		m_data_array(_data_array),
		m_current_absolute_index(NONE),
		m_last_datum_index(DATUM_INDEX_NONE)
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
			m_last_datum_index = DATUM_INDEX_NONE;
			m_current_absolute_index = m_data_array->datum_max_elements;
		}
		else
		{
			result = reinterpret_cast<T*>(&m_data_array->data[m_data_array->datum_element_size * index]);
			m_current_absolute_index = index;
			m_last_datum_index = DATUM_INDEX_NEW(index, *(unsigned short*)(result)); // absolute index w/ salt
		}
		return result;
	}
	
	int get_next_absolute_datum_index(int index) const
	{
		if (index < 0)
			return -1;

		if (index >= m_data_array->next_unused_index)
			return -1;

		while (!m_data_array->active_bit_mask.test_bit(index))
		{
			if (++index >= m_data_array->next_unused_index)
				return -1;
		}
		return index;
	}

	int get_current_absolute_index() const
	{
		return m_current_absolute_index;
	}

	datum get_current_datum_index() const
	{
		return m_last_datum_index;
	}

	int get_data_count() const
	{
		return m_data_array->total_elements_used;
	}

private:
	
	s_data_array* m_data_array;
	datum m_last_datum_index;
	int m_current_absolute_index;
};

// Get data in s_data_array from datum index
void* datum_get(s_data_array* data, datum index);

void __cdecl datum_delete(s_data_array* data_array, datum object_index);
