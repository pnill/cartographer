#pragma once

#include "H2MOD/Tags/TagInterface.h"
class LazyBuffer
{
public:
	char* buffer;
	size_t size = 0;

	explicit LazyBuffer() : buffer(nullptr) {}
	~LazyBuffer() { free(buffer); }

	void resize(size_t newSize)
	{
		if (auto mem = realloc(buffer, newSize))
		{
			buffer = static_cast<char*>(mem);
			size = newSize;
		}
		else
			throw std::bad_alloc();
	}

	/**
	 * \brief Method Assumes you are trying to access the end of the buffer and takes an input for how much data you are looking to store
	 * \param expectedAvail The size of the data you are looking to store
	 * \return Returns a pointer to the start of the new data.
	 */
	char* next(size_t expectedAvail)
	{
		resize(size + expectedAvail);
		return &buffer[size - expectedAvail];
	}
	char* operator[](size_t pos) const
	{
		if (pos >= size)
			throw std::out_of_range("Index out of range");
		return &buffer[pos];
	}
	template<typename T>
	T* as(size_t position = 0)
	{
		return reinterpret_cast<T*>(&buffer[position]);
	}
};
struct lazy_blam_tag_instance : tags::tag_instance
{
	std::string name;
	LazyBuffer data;
	lazy_blam_tag_instance(tags::tag_instance instance, std::string name)
	{
		this->datum_index = instance.datum_index;
		this->data_offset = instance.data_offset;
		this->size = instance.size;
		this->type = instance.type;
		this->name = name;
	}
};

struct s_tag_table_data
{
	unsigned int tag_table_start_unpadded;
	unsigned int tag_table_start;
	unsigned int tag_table_end;
	unsigned int tag_count;
	unsigned int scenario_address;
	unsigned int tag_data_start;
	std::vector<lazy_blam_tag_instance> table;
};
