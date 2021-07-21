#pragma once
#include "H2MOD/Tags/TagInterface.h"
#include "TagTable.h"
#include "Loader/Biped.hpp"


namespace lazy_blam
{
	bool init_cache_file(std::string map_name);
	void close_cache_file();
	datum get_datum_from_name(std::string tag_name, blam_tag type);
	char* init_tag_data(datum tag_index);
	tags::tag_instance* get_tag_instance(datum tag_datum);
	std::string get_name_from_datum(datum tag_datum);
	std::ifstream* get_map_stream();
	s_cache_header* get_cache_header();
	s_tag_table_data* get_tag_table();
	void clear_loaded_tags();

	template<typename T>
	T* get_tag_data(datum tag_datum)
	{
		auto map_stream = get_map_stream();
		auto map_header = get_cache_header();
		auto tag_table = get_tag_table();

		tags::tag_instance* tag_inst = get_tag_instance(tag_datum);
		//int tag_data_offset;
		//if(map_header->type == s_cache_header::MultiplayerSharedScenario)
		//	tag_data_offset = tag_table->tag_data_start + (tag_inst->data_offset - 0x3c000);
		//else
		//	tag_data_offset = tag_table->tag_data_start + (tag_inst->data_offset - tag_table->scenario_address);

		auto data = init_tag_data(tag_datum);

		//map_stream->seekg(tag_data_offset);
		//map_stream->read(data, tag_inst->size);


		auto a = biped_loader();
		a.init(tag_inst, map_stream, map_header, tag_table);
		auto test = reinterpret_cast<s_biped_group_definition*>(a.get_tag_data(0));
		LOG_INFO_GAME("[{}] {} {}", __FUNCTION__, test->unitTag.objectTag.ai_properties.data);

		return (T*)data;
	}

};

