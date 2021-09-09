#pragma once
#include "H2MOD/Modules/Utils/Utils.h"

namespace MetaExtender
{
	void add_tag_block(unsigned long tag_block_ptr, int tag_block_item_size, int tag_block_item_count, int newCount);
	void test();
	void free_tag_blocks();
	void add_to_free(void* mem);
	template<typename T = void>
	T* copy_tag_block(unsigned long tag_block_ptr, int src_index, int dst_index = -1)
	{

		int* block_count = reinterpret_cast<int*>(tag_block_ptr);
		int* block_offset = reinterpret_cast<int*>(tag_block_ptr + 4);
		if (src_index > *block_count)
			return nullptr;
		if (dst_index > *block_count)
			return nullptr;
		size_t block_size = *block_count * sizeof(T*);

		void* new_memory;
		if (dst_index == -1)
			new_memory = calloc(block_size + 1, sizeof(T));
		else
			new_memory = calloc(block_size, sizeof(T));

		add_to_free(new_memory);

		memcpy(new_memory, &tags::get_tag_data()[*block_offset], block_size);

		if (dst_index == -1)
		{
			memcpy((void*)(((unsigned long)new_memory) + (sizeof(T) * (*block_count - 1))),
				(void*)(((unsigned long)new_memory) + (sizeof(T) * src_index)),
				sizeof(T));
			*block_count = *block_count + 1;
			*block_offset = (int)((unsigned long)new_memory - int(*Memory::GetAddress<int**>(0x47CD54)));
			LOG_INFO_GAME(IntToString<int>(*block_offset, std::hex));
			return reinterpret_cast<T*>(((unsigned long)new_memory) + (sizeof(T) * (*block_count - 1)));
		}
		else
		{
			memcpy((void*)(((unsigned long)new_memory) + (sizeof(T) * dst_index)),
				(void*)(((unsigned long)new_memory) + (sizeof(T) * src_index)),
				sizeof(T));
			*block_offset = (int)((unsigned long)new_memory - int(*Memory::GetAddress<int**>(0x47CD54)));
			LOG_INFO_GAME(IntToString<int>(*block_offset, std::hex));
			return reinterpret_cast<T*>(((unsigned long)new_memory) + (sizeof(T) * dst_index));
		}

	}
	template<typename T = void>
	T* add_tag_block2(unsigned long tag_block_ptr)
	{
		int* block_count = reinterpret_cast<int*>(tag_block_ptr);
		int* block_offset = reinterpret_cast<int*>(tag_block_ptr + 4);
		size_t block_size = *block_count * sizeof(T);
		void* new_memory = calloc(*block_count + 1, sizeof(T));
		add_to_free(new_memory);

		memcpy(new_memory, &tags::get_tag_data()[*block_offset], block_size);

		*block_count = *block_count + 1;
		*block_offset = (int)((unsigned long)new_memory - int(*Memory::GetAddress<int**>(0x47CD54)));

		return reinterpret_cast<T*>(((unsigned long)new_memory) + (sizeof(T) * (*block_count - 1)));
	}
}
