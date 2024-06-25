#pragma once
#include "H2MOD/Tags/TagInterface.h"

// ### TODO Rename to c_tag_block and refactor

template<typename T = void>
struct tag_block
{
	int32 count;
	int32 data;

	int32 data_size() const
	{
		return count * type_size();
	}
	int32 type_size() const
	{
		return sizeof(T);
	}
	T* begin() const
	{
		if (this->data != NONE)
		{
			char* tag_data_table = tags::get_tag_data();
#ifdef DEBUG
			if (LOG_CHECK(tag_data_table))
#endif
				return reinterpret_cast<T*>(&tag_data_table[this->data]);
		}
		return nullptr;
	}

	T* end() const
	{
		static_assert(std::is_void<T>::value == false, "You need to set the tag block type to use this function");
		if (this->begin())
			return &this->begin()[this->count];
		else
			return nullptr;
	}

	T* operator[](int32 index) const
	{
		static_assert(std::is_void<T>::value == false, "You need to set the tag block type to use this function");
		if (index == NONE)
			return NULL;
		if (index >= this->count)
			return NULL;
		if (this->begin()) {
			T* data_array = this->begin();
			return &data_array[index];
		}
		return NULL;
	}
};
ASSERT_STRUCT_SIZE(tag_block<>, 8);

struct s_tag_block_index
{
	int16 count;
};
ASSERT_STRUCT_SIZE(s_tag_block_index, 2);
