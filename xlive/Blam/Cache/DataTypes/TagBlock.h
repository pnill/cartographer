#pragma once

template<typename T = void>
struct tag_block
{
	int32 size;
	int32 data;

	int data_size() const
	{
		return size * sizeof(T);
	}
	int type_size() const
	{
		return sizeof(T);
	}
	T *begin() const
	{
		if (this->data != NONE)
		{
			char *tag_data_table = tags::get_tag_data();
#ifdef DEBUG
			if (LOG_CHECK(tag_data_table))
#endif
				return reinterpret_cast<T*>(&tag_data_table[this->data]);
		}
		return nullptr;
	}

	T* end() const
	{
#ifdef DEBUG
		static_assert(std::is_void<T>::value == false, "You need to set the tag block type to use this function");
#endif
		if (this->begin())
			return &this->begin()[this->size];
		else
			return nullptr;
	}

	T* operator[](int32 index) const
	{
#ifdef DEBUG
		static_assert(std::is_void<T>::value == false, "You need to set the tag block type to use this function");
#endif
		if (index == NONE)
			return nullptr;
		if (index >= this->size)
			return nullptr;
		if (this->begin()) {
			T *data_array = this->begin();
			return &data_array[index];
		}
		return nullptr;
	}
};