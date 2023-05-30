#include "stdafx.h"

#include "MetaExtender.h"
#include "TagInterface.h"

std::vector<void*> ToFree;

namespace MetaExtender {
	template<typename T = void>
	void move_child_block(unsigned long tag_block_ptr)
	{
		int* block_count = reinterpret_cast<int*>(tag_block_ptr);
		int* block_offset = reinterpret_cast<int*>(tag_block_ptr + 4);
		size_t block_size = *block_count * sizeof(T);
		void* new_memory = calloc(*block_count + 1, sizeof(T));
		ToFree.push_back(new_memory);
		memcpy(new_memory, &tags::get_tag_data()[*block_offset], block_size);
		if (visit_struct::traits::is_visitable<T>::value)
		{
			for (auto i = 0; i < *block_count; i++)
			{
				const T* t_block = reinterpret_cast<T*>(((unsigned long)new_memory) + (sizeof(T) * i));
				visit_struct::for_each(*t_block, [](const char *name, const auto &value)
				{
					if (std::string(typeid(value).name()).find("tag_block") != std::string::npos)
					{
						auto a = reinterpret_cast<decltype(value)>(value);
						LOG_INFO_GAME(IntToString<unsigned long>((unsigned long)std::addressof(value), std::hex));

						if (a.size != 0)
						{
							move_child_block<decltype(a[0])>((unsigned long)std::addressof(value));
						}
					}
				});
			}
		}
	}
	//Currently only works with 1 level deep reflexives.
	void add_tag_block(unsigned long tag_block_ptr, int tag_block_item_size, int tag_block_item_count, int newCount)
	{
		size_t currentSize = tag_block_item_size * tag_block_item_count;

		void* new_tag_block = calloc(tag_block_item_count + newCount, tag_block_item_size);

		ToFree.push_back(new_tag_block);

		memcpy(new_tag_block, &tags::get_tag_data()[*reinterpret_cast<int*>(tag_block_ptr + 4)], currentSize);

		*reinterpret_cast<int*>(tag_block_ptr) = tag_block_item_count + newCount;
		*reinterpret_cast<int*>(tag_block_ptr + 4) = (unsigned long)new_tag_block - int(*Memory::GetAddress<int**>(0x47CD54));
	}


	//template<typename T = void>
	//T* add_tag_block2(unsigned long tag_block_ptr)
	//{
	//	int* block_count = reinterpret_cast<int*>(tag_block_ptr);
	//	int* block_offset = reinterpret_cast<int*>(tag_block_ptr + 4);
	//	size_t block_size = *block_count * sizeof(T);
	//	void* new_memory = calloc(*block_count + 1, sizeof(T));
	//	ToFree.push_back(new_memory);

	//	memcpy(new_memory, &tags::get_tag_data()[*block_offset], block_size);

	//	*block_count = *block_count + 1;
	//	*block_offset = (int)((unsigned long)new_memory - int(*Memory::GetAddress<int**>(0x47CD54)));

	//	return reinterpret_cast<T*>(((unsigned long)new_memory) + (sizeof(T) * (*block_count - 1)));
	//}
	template<typename T = void>
	T* add_tag_block3(tag_block<T>* block)
	{
		size_t block_size = block->size * sizeof(T);
		void* new_memory = calloc(block->size + 1, sizeof(T));
		ToFree.push_back(new_memory);

		memcpy(new_memory, &tags::get_tag_data()[block->data], block_size);

		block->size = block->size + 1;
		block->data = (int)((unsigned long)new_memory - int(*Memory::GetAddress<int**>(0x47CD54)));

		return reinterpret_cast<T*>(((unsigned long)new_memory) + (sizeof(T) * (block->size - 1)));
	}

	void free_tag_blocks()
	{
		for (auto &block : ToFree)
		{
			free(block);
		}
		ToFree.clear();
	}

	void add_to_free(void* mem)
	{
		ToFree.push_back(mem);
	}
}
