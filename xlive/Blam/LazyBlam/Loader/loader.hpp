#pragma once
#include "H2MOD/Tags/TagInterface.h"
#include "Blam/LazyBlam/TagTable.h"
#include "Blam/Cache/DataTypes/TagBlock.h"

#define L_BLAM_LOADER_REBASE(tag_block) if(tag_block.data > 0) (tag_block.data = base + (tag_block.data - instance->data_offset))
#define L_BLAM_LOADER_READ_BLOCK(object, tag_block, tag_block_index, type) \
	map_stream->seekg(resolve_data_offset(tag_block.data) + tag_block_index * sizeof(type)); \
	map_stream->read((char*)&object, sizeof(type))
#define L_BLAM_LOADER_CAST_BLOCK(tag_block, type) \
	reinterpret_cast<type*>(&(*this->data)[tag_block.data])

class BlamLoader
{
public:
	void init(tags::tag_instance* instance, std::ifstream* map_stream, s_cache_header* header, s_tag_table_data* tag_table)
	{
		this->instance = instance;
		this->map_stream = map_stream;
		this->map_header = header;
		this->tag_table = tag_table;
		this->base_data = new char[instance->size];
		for(auto &inst : tag_table->table)
			if (std::get<0>(inst).datum_index == instance->datum_index)
				this->data = &std::get<2>(inst);
		map_stream->seekg(resolve_data_offset(instance->data_offset));
		map_stream->read(this->base_data, instance->size);
	}

	unsigned int resolve_data_offset(unsigned int offset)
	{
		switch (map_header->type)
		{
		case s_cache_header::SinglePlayerScenario:
		case s_cache_header::MultiplayerScenario:
		case s_cache_header::MainMenuScenario:
			return tag_table->tag_data_start + (offset - tag_table->scenario_address);
		case s_cache_header::SinglePlayerSharedScenario:
		case s_cache_header::MultiplayerSharedScenario:
			return tag_table->tag_data_start + (offset - 0x3c000);
		}
		return -1;
	}
	//template<typename T>
	//T* cast_tag_block(tag_block<T> tag_block)
	//{
	//	return reinterpret_cast<T*>(&this->data[tag_block.data]);
	//}
	std::ifstream* map_stream;
	s_cache_header* map_header;
	s_tag_table_data* tag_table;
	tags::tag_instance* instance;
	char* base_data;
	char** data;
	int current_position = 0;
};
