#include "stdafx.h"
#include "LazyBlam.hpp"
#include "H2MOD\Modules\Utils\Utils.h"
#include "Util\filesys.h"

namespace lazy_blam
{
	static datum matg_datum(0xE1742710);

	//std::string map_file;
	//std::ifstream* map_stream;
	//std::string maps_dir;
	//std::string mods_dir;
	//
	//s_cache_header map_header;
	//datum scenario_datum;
	//s_tag_table_data tag_table;

	bool lazy_blam::init_cache_file(std::string map_name)
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
			tag_table.tag_table_start = map_header.tag_offset + (0xC * tag_parent_count + 0x20) + DATUM_INDEX_TO_ABSOLUTE_INDEX(matg_datum) * sizeof(tags::tag_instance);
			tag_table.tag_data_start = 0x3c000 + map_header.tag_offset;
		}
		else
		{
			tag_table.tag_table_start = tag_table.tag_table_start_unpadded;
			tag_table.tag_data_start = map_header.tag_size + map_header.tag_offset;

		}

		
		map_stream->seekg(tag_table.tag_table_start + DATUM_INDEX_TO_ABSOLUTE_INDEX(scenario_datum) * sizeof(tags::tag_instance) + 0x8);
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

			//tag_table.table.emplace_back(tag_instance, input, nullptr);
			tag_table.table.push_back(lazy_blam_tag_instance(tag_instance, input));

			++count;
		}
		tag_table.tag_count = count;
		tag_table.tag_table_end = tag_table.tag_table_start + count * sizeof(tags::tag_instance);
		LOG_INFO_GAME("[{}] {} {:x}-{:x}", __FUNCTION__, map_header.scenario_path, tag_table.tag_table_start, tag_table.tag_table_end);
		return true;
	}

	void lazy_blam::close_cache_file()
	{
		map_stream->close();
		delete map_stream;
		map_header = s_cache_header();
	}

	datum lazy_blam::get_datum_from_name(std::string tag_name, blam_tag type)
	{
		for (auto& instance : tag_table.table)
			if (instance.name == tag_name)
				return instance.datum_index;
		return -1;
	}



	lazy_blam_tag_instance* lazy_blam::get_tag_instance(datum tag_datum)
	{
		for (auto& instance : tag_table.table)
			if (instance.datum_index == tag_datum)
				return &instance;
		return nullptr;
	}

	std::string lazy_blam::get_name_from_datum(datum tag_datum)
	{
		return "";
	}

	std::ifstream* lazy_blam::get_map_stream()
	{
		return map_stream;
	}

	s_cache_header* lazy_blam::get_cache_header()
	{
		return &map_header;
	}

	s_tag_table_data* lazy_blam::get_tag_table()
	{
		return &tag_table;
	}

	void lazy_blam::clear_loaded_tags()
	{
		//for(auto& instance : tag_table.table)
		//	if(std::get<2>(instance) != nullptr)
		//		free(std::get<2>(instance));
	}
	unsigned int lazy_blam::resolve_data_offset(unsigned int offset)
	{
		auto ret = -1;
		switch (map_header.type)
		{
			case s_cache_header::SinglePlayerScenario:
			case s_cache_header::MultiplayerScenario:
			case s_cache_header::MainMenuScenario:
				ret = tag_table.tag_data_start + (offset - tag_table.scenario_address);
			case s_cache_header::SinglePlayerSharedScenario:
			case s_cache_header::MultiplayerSharedScenario:
				ret = tag_table.tag_data_start + (offset - 0x3c000);
		}
		//LOG_TRACE_GAME("[{}] {:x}", __FUNCTION__, ret);
		return ret;
	}

	std::vector<datum> lazy_blam::find_tags(blam_tag type)
	{
		std::vector<datum> result;
		for(auto &it = tag_table.table.begin(); it != tag_table.table.end(); it++)
			if(it->type == type)
				result.emplace_back(it->datum_index);
		return result;
	}


	char* lazy_blam::load_tag_data(lazy_blam_tag_instance* instance)
	{
		if (instance->data.size == 0) {
			map_stream->seekg(resolve_data_offset(instance->data_offset));
			map_stream->read(instance->data.next(instance->size), instance->size);
			switch (instance->type.tag_type) {
				case blam_tag::tag_group_type::biped:
					break;
				case blam_tag::tag_group_type::vehicle:
					break;
				case blam_tag::tag_group_type::weapon:
					loader::weapon(this, instance);
					break;
				case blam_tag::tag_group_type::globals:
					break;
				default: ;
			}
			return instance->data[0];
		}
		return nullptr;
	}

	void lazy_blam::rebase_tag_data(lazy_blam_tag_instance* instance)
	{
		auto tag_data_base = int(*Memory::GetAddress<int**>(0x47CD54));
		unsigned long newBase = (unsigned long)((unsigned long)instance->data.buffer - tag_data_base);
		if (newBase == instance->data_offset)
			return;
		LOG_INFO_GAME("[{}] {:x} {:x} {:x} {}", __FUNCTION__, tag_data_base, (unsigned long)instance->data.buffer, newBase, instance->name);
		switch (instance->type.tag_type)
		{
			case blam_tag::tag_group_type::biped:
				rebase::biped(instance, newBase);
				break;
			case blam_tag::tag_group_type::vehicle:
				rebase::vehicle(instance, newBase);
				break;
			case blam_tag::tag_group_type::weapon:
				rebase::weapon(instance, newBase);
				break;
			case blam_tag::tag_group_type::globals:
				rebase::globals(instance, newBase);
				break;
			default:;
		}
		instance->data_offset = newBase;
	}
}
