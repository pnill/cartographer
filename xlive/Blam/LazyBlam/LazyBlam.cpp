#include "stdafx.h"
#include "LazyBlam.hpp"
#include "H2MOD/Modules/Utils/Utils.h"
#include "Util/filesys.h"

namespace lazy_blam
{
	datum matg_datum(0xE1742710);

	std::string map_file;
	std::ifstream* map_stream;
	std::string maps_dir;
	std::string mods_dir;
	
	s_cache_header map_header;
	datum scenario_datum;
	s_tag_table_data tag_table;
	
	bool init_cache_file(std::string map_name)
	{
		maps_dir = GetExeDirectoryNarrow() + "\\maps";
		mods_dir = GetExeDirectoryNarrow() + "\\mods";
		if (FileTypeCheck(map_name, "map"))
			map_file = mods_dir + "\\maps\\" + map_name;
		else
			map_file = mods_dir + "\\maps\\" + map_name + ".map";

		if (PathFileExistsA(map_file.c_str()));
		else
		{
			if (FileTypeCheck(map_name, "map"))
				map_file = maps_dir + '\\' + map_name;
			else
				map_file = maps_dir + '\\' + map_name + ".map";

			if (!PathFileExistsA(map_file.c_str()))
				return false;
		}


		map_stream = new std::ifstream(map_file.c_str(), std::ios::binary | std::ios::in);

		map_stream->read((char*)&map_header, sizeof(s_cache_header));
		map_stream->seekg(map_header.tag_offset + 4);
		int tag_parent_count;
		map_stream->read((char*)&tag_parent_count, sizeof(int));
		map_stream->seekg(map_header.tag_offset + 12);
		map_stream->read((char*)&scenario_datum, sizeof(datum));
		tag_table = s_tag_table_data();

		tag_table.tag_table_start_unpadded = map_header.tag_offset + 0xC * tag_parent_count + 0x20;;
		if (map_header.type == s_cache_header::e_scnr_type::MultiplayerSharedScenario)
		{
			tag_table.tag_table_start = map_header.tag_offset + (0xC * tag_parent_count + 0x20) + matg_datum.Index * sizeof(tags::tag_instance);
			tag_table.tag_data_start = 0x3c000 + map_header.tag_offset;
		}
		else
		{
			tag_table.tag_table_start = tag_table.tag_table_start_unpadded;
			tag_table.tag_data_start = map_header.tag_size + map_header.tag_offset;

		}

		
		map_stream->seekg(tag_table.tag_table_start + scenario_datum.Index * sizeof(tags::tag_instance) + 0x8);
		map_stream->read((char*)&tag_table.scenario_address, 4);

		tags::tag_instance tag_instance;
		std::string input;
		int count = 0;
		int names_pos = map_header.TagNamesBufferOffset;

		while (true) 
		{
			map_stream->seekg(tag_table.tag_table_start + count * sizeof(tags::tag_instance));
			map_stream->read((char*)&tag_instance, sizeof(tags::tag_instance));

			if (!tag_instance.type.is_class_valid())
				break;

			map_stream->seekg(names_pos);
			std::getline(*map_stream, input, '\0');
			names_pos = map_stream->tellg();

			tag_table.table.emplace_back(tag_instance, input, nullptr);

			++count;
		}
		tag_table.tag_count = count;
		tag_table.tag_table_end = tag_table.tag_table_start + count * sizeof(tags::tag_instance);
		LOG_INFO_GAME("[{}] {} {:x}-{:x}", __FUNCTION__, map_header.scenario_path, tag_table.tag_table_start, tag_table.tag_table_end);
		return true;
	}

	void close_cache_file()
	{
		map_stream->close();
		delete map_stream;
		map_header = s_cache_header();
	}

	datum get_datum_from_name(std::string tag_name, blam_tag type)
	{
		for (auto& instance : tag_table.table)
			if (std::get<1>(instance) == tag_name)
				return std::get<0>(instance).datum_index;
		return datum::Null;
	}

	char* init_tag_data(datum tag_index)
	{
		for (auto& instance : tag_table.table)
			if(std::get<0>(instance).datum_index == tag_index)
			{
				if (std::get<2>(instance) == nullptr) 
					std::get<2>(instance) = new char[std::get<0>(instance).size];
				
				return std::get<2>(instance);
			}
		return nullptr;
	}

	tags::tag_instance* get_tag_instance(datum tag_datum)
	{
		for(auto& instance : tag_table.table)
			if (std::get<0>(instance).datum_index == tag_datum)
				return &std::get<0>(instance);
		return nullptr;
	}

	std::string get_name_from_datum(datum tag_datum)
	{
		return "";
	}

	std::ifstream* get_map_stream()
	{
		return map_stream;
	}

	s_cache_header* get_cache_header()
	{
		return &map_header;
	}

	s_tag_table_data* get_tag_table()
	{
		return &tag_table;
	}

	void clear_loaded_tags()
	{
		for(auto& instance : tag_table.table)
			if(std::get<2>(instance) != nullptr)
				free(std::get<2>(instance));
	}
	unsigned int resolve_data_offset(unsigned int offset)
	{
		switch (map_header.type)
		{
		case s_cache_header::SinglePlayerScenario:
		case s_cache_header::MultiplayerScenario:
		case s_cache_header::MainMenuScenario:
			return tag_table.tag_data_start + (offset - tag_table.scenario_address);
		case s_cache_header::SinglePlayerSharedScenario:
		case s_cache_header::MultiplayerSharedScenario:
			return tag_table.tag_data_start + (offset - 0x3c000);
		}
		return -1;
	}
}
