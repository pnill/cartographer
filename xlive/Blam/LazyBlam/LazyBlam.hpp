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
		//TODO: Can the initializer for blam_loader be passed to biped_loader? Maybe transfer blam_loader to just be part of Lazy_Blam.. Simplify
		auto a = biped_loader();
		a.init(tag_inst, map_stream, map_header, tag_table);
		//TODO: Rebase based off the actual tag_data address, would be a bit of a hack but would work for accessing tag_blocks without any issues..
		return a.get_tag_data(0);
	}

};

