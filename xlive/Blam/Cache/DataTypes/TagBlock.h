#pragma once
#ifndef TAGBLOCK_H
#define TAGBLOCK_H

#include "..\H2MOD\Tags\TagInterface.h"

extern DWORD H2BaseAddr;
template<typename T = void>
struct tag_block
{
	size_t size;
	size_t data;

	T *begin()
	{
		if (this->data != NONE)
		{
			char *tag_data_table = tags::get_tag_data();
			if (LOG_CHECK(tag_data_table))
				return reinterpret_cast<T*>(&tag_data_table[this->data]);
		}
		return nullptr;
	}

	T *end()
	{
		static_assert(std::is_void<T>::value == false, "You need to set the tag block type to use this function");
		if (this->begin())
			return &this->begin()[this->size];
		else
			return nullptr;
	}

	T *operator[](size_t index)
	{
		static_assert(std::is_void<T>::value == false, "You need to set the tag block type to use this function");
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
#endif