#include "stdafx.h"
#include "tag_loader.h"

#include "cache/cache_files.h"
#include "game/game_options.h"
#include "scenario/scenario.h"
#include "render/weather_definitions.h"

#include "Blam/Cache/TagGroups/scenery_definition.hpp"

#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "Util/filesys.h"

// TODO Cleanup

//contains some game functions that returns HANDLE
namespace global_handle_function
{
	HANDLE __cdecl get_map_Handle_from_scnr(const char* pScenario)
	{
		return	((HANDLE(__cdecl*)(const char*))Memory::GetAddress(0x38607))(pScenario);
	}
}
//certain functions which relate tags to their global object(such as Havok objects)(vftables perhaps)
namespace global_objects_fix
{
	void __cdecl bipd_fix(datum datum_index)
	{
		void(_cdecl * sub_EC23F)(datum);
		sub_EC23F = (void(_cdecl*)(datum))Memory::GetAddress(0x1389B0);
		sub_EC23F(datum_index);
	}

	void __cdecl crea_fix(datum datum_index)
	{
		int(_cdecl * sub_EC23F)(datum);
		sub_EC23F = (int(_cdecl*)(datum))Memory::GetAddress(0x138985);
		sub_EC23F(datum_index);
	}
	void __cdecl vehi_fix(datum datum_index)
	{
		int(_cdecl * sub_EC23F)(datum);
		sub_EC23F = (int(_cdecl*)(datum))Memory::GetAddress(0x13895A);
		sub_EC23F(datum_index);
	}
	void __cdecl coll_fix(datum datum_index)
	{
		int(_cdecl * sub_EC23F)(datum);
		sub_EC23F = (int(_cdecl*)(datum))Memory::GetAddress(0x7BE5C);
		sub_EC23F(datum_index);
	}
	void __cdecl phmo_fix(datum datum_index, bool unk)
	{
		int(_cdecl * sub_EC23F)(datum, bool);
		sub_EC23F = (int(_cdecl*)(datum, bool))Memory::GetAddress(0x7B844);
		sub_EC23F(datum_index, unk);
	}
}
//actual tag_loader namespace
namespace tag_loader
{
	std::string maps_directory;
	std::string custom_maps_directory;
	std::string custom_tags_directory;
	std::string plugins_directory;
	std::string mods_directory;

	static std::map<std::string, std::shared_ptr<plugins_field>> plugins_list;//contains list of various plugin structures
	std::map<int, std::shared_ptr<meta>> que_meta_list;//<datum_index,meta_junk>contains the list of tags that are currently in que to loaded in memory
	std::vector<int> key_list;//another var just to keep the keys along with the correct order
	std::vector<int> injected_list;
	//map<int, shared_ptr<meta>> meta_list;//<datum_index,meta_junk>contains the the list of tags that are currently loaded in memory(never gonna use it anyway)
	std::vector<std::string> error_list;//contains various messages generated during various processes,shouldnt had named it error list
	std::vector<std::string> tag_list;//contains a list of tag_indices along with their names(currently implemented only for module loading)
	std::list<injectRefs> injected_tag_refs;
	unsigned int def_meta_size = 0;
	unsigned int used_additional_meta = 0;
	unsigned int new_datum_index = _INJECTED_TAG_START_;//first datum index
	tags::tag_instance* reallocated_tag_table;//new tables pointer

	std::string meta_list_target_map;//name of the map for which the meta list has been target(used for StringIDs)
	std::string unused_target_map_name;//name of the map from which the que list has been loaded(used for StringID retargeting)
	std::vector<DWORD> sync_list;//container to store tag indices of sync-able Tags

	std::shared_ptr<plugins_field> Get_plugin(std::string type)
	{
		//we first look into the already loaded plugin list
		for (auto& i : plugins_list)
		{
			if (i.first == type)
				return i.second;
		}

		type.erase(remove(type.begin(), type.end(), ' '), type.end());
		std::string plugin_loc = plugins_directory + '\\' + type + ".xml";
		std::shared_ptr<plugins_field> temp_plugin = meta_struct::Get_Tag_stucture_from_plugin(plugin_loc);

		if (temp_plugin)
			plugins_list.emplace(type, temp_plugin);
		else
		{
			std::string error = "Couldnt load plugin " + type + ".xml";
			throw error;
			//error_list.push_back(error);
		}

		return temp_plugin;
	}
	//returns whether the map is a shared map or not
	bool Check_shared(std::ifstream* fin)
	{
		char* map_header = new char[0x800];
		fin->seekg(0x0);
		fin->read(map_header, 0x800);

		if (cache_files_get_header()->type == _game_mode_mutiplayer_shared || cache_files_get_header()->type == _game_mode_single_player_shared)
		{
			delete[] map_header;
			return true;
		}
		delete[] map_header;
		return false;
	}

	bool Map_exists(std::string map)
	{
		std::string map_loc;
		if (meta_struct::Get_file_type(map) == "map")
			map_loc = mods_directory + "\\maps\\" + map;
		else
			map_loc = mods_directory + "\\maps\\" + map + ".map";

		if (PathFileExistsA(map_loc.c_str()))
		{
			return true;
		}
		if (meta_struct::Get_file_type(map) == "map")
			map_loc = maps_directory + '\\' + map;
		else
			map_loc = maps_directory + '\\' + map + ".map";

		if (PathFileExistsA(map_loc.c_str()))
		{
			return true;
		}

		if (meta_struct::Get_file_type(map) == "map")
			map_loc = custom_maps_directory + '\\' + map;
		else
			map_loc = custom_maps_directory + '\\' + map + ".map";

		return PathFileExistsA(map_loc.c_str());

	}

	void find_map_path(const char* map_name, c_static_string260* out_string)
	{
		//logic to check the existence of the map at subsequent directories
		//mods->default->custom
		out_string->clear();
		out_string->set(mods_directory.c_str());
		out_string->append("\\maps\\");
		if (meta_struct::Get_file_type(map_name) == "map")
		{
			out_string->append(map_name);
		}
		else
		{
			out_string->append(map_name);
			out_string->append(".map");
		}

		if (PathFileExistsA(out_string->get_string())){}
		else
		{
			out_string->set(maps_directory.c_str());
			out_string->append("\\");
			if (meta_struct::Get_file_type(map_name) == "map")
			{
				out_string->append(map_name);
			}
			else
			{
				out_string->append(map_name);
				out_string->append(".map");
			}

			if (PathFileExistsA(out_string->get_string()));
			else
			{
				out_string->set(custom_maps_directory.c_str());
				out_string->append("\\");
				if (meta_struct::Get_file_type(map_name) == "map")
				{
					out_string->append(map_name);
				}
				else
				{
					out_string->append(map_name);
					out_string->append(".map");
				}
			}
		}
	}

	datum get_tag_datum_by_name(std::string tag_name, e_tag_group type, std::string map)
	{
		c_static_string260 map_location;
		find_map_path(map.c_str(), &map_location);

		std::ifstream* file_stream = new std::ifstream(map_location.get_string(), std::ios::binary | std::ios::in);

		if (file_stream->is_open())
		{
			s_cache_header cache_header;
			s_tags_header tags_header;

			file_stream->seekg(0);
			file_stream->read((char*)&cache_header, sizeof(s_cache_header));

			file_stream->seekg(cache_header.tag_offset);
			file_stream->read((char*)&tags_header, sizeof(s_tags_header));

			int instance_table_offset = cache_header.tag_offset + 0xC * tags_header.tag_parent_info_count + 0x20;

			tags::tag_instance current_tag_instance;

			file_stream->seekg(cache_header.tag_name_buffer_offset);
			int current_position = 0;
			char ch = ' ';
			std::string input;

			// iterate through all the tag_names till the requested one is found
			for (auto i = 0; i < cache_header.tag_name_count; i++)
			{
				// grab the next line till a null terminator is found
				std::getline(*file_stream, input, '\0');
				if (input == tag_name)
				{
					// store the current position of the stream inside the file name table
					current_position = file_stream->tellg();

					// seek and grab the tag_instance that matches the current file name index
					file_stream->seekg(instance_table_offset + i * sizeof(tags::tag_instance));
					file_stream->read((char*)&current_tag_instance, sizeof(tags::tag_instance));

					// if the tag group of the requested tag matches we found it
					if (current_tag_instance.type.group == type)
					{
						delete file_stream;
						return current_tag_instance.datum_index;
					}
					file_stream->seekg(current_position);
				}
			}
		}

		// no match found return none
		file_stream->close();
		delete file_stream;
		return NONE;
	}
	//Loads a tag from specified map in accordance with the datum index supplied
	///custom flag is no more needed
	void Load_tag(int datum_index, bool recursive, std::string map, bool custom)
	{
		unused_target_map_name = map;

		c_static_string260 map_location;
		find_map_path(map.c_str(), &map_location);

		std::ifstream* file_stream = new std::ifstream(map_location.get_string(), std::ios::binary | std::ios::in);

		if (file_stream->is_open())
		{
			std::string temp_string = "Loading tag : " + meta_struct::to_hex_string(datum_index) + " from " + map;
			error_list.push_back(temp_string);

			s_cache_header cache_header;
			s_tags_header tags_header;

			file_stream->seekg(0);
			file_stream->read((char*)&cache_header, sizeof(s_cache_header));

			file_stream->seekg(cache_header.tag_offset);
			file_stream->read((char*)&tags_header, sizeof(s_tags_header));

			int instance_table_offset = cache_header.tag_offset + 0xC * tags_header.tag_parent_info_count + 0x20;
			int tag_data_start_offset = cache_header.tag_offset + cache_header.data_offset;

			tags::tag_instance scenario_instance;
			file_stream->seekg(instance_table_offset);
			file_stream->read((char*)&scenario_instance, sizeof(tags::tag_instance));

			int scnr_memaddr;
			file_stream->seekg(instance_table_offset + 0x8);
			file_stream->read((char*)&scnr_memaddr, 4);

			tags::tag_instance tag_info;

			//create a tag_list to load
			std::list<int> load_tag_list;
			load_tag_list.push_back(datum_index);

			//----------------LOOPING STUFF
			while (load_tag_list.size())
			{
				if (*load_tag_list.cbegin() != -1 && *load_tag_list.cbegin() != 0)
				{
					//method to read tag type
					file_stream->seekg(instance_table_offset + (0xFFFF & *(load_tag_list.cbegin())) * sizeof(tags::tag_instance));

					file_stream->read((char*)&tag_info, sizeof(tags::tag_instance));

					if (*(load_tag_list.cbegin()) == tag_info.datum_index && tag_info.type.group != _tag_group_sound)
					{
						tag_group group_name = tag_group_get_name(tag_info.type);
						char null_terminated_string[5] = { group_name.string[0], group_name.string[1], group_name.string[2], group_name.string[3], '\0' };
						std::shared_ptr<plugins_field> temp_plugin = Get_plugin(null_terminated_string);

						//precaution for plugin load errors
						if (!temp_plugin)
							break;

						//we first check the integrity of the datum_index
						if (tag_info.datum_index == *(load_tag_list.cbegin()) && tag_info.data_offset && tag_info.size > 0 && (que_meta_list.find(tag_info.datum_index) == que_meta_list.cend()))
						{
							//read the meta data from the map            
							char* data = new char[tag_info.size];

							int map_off;
							if (!Check_shared(file_stream))
								map_off = tag_data_start_offset + (tag_info.data_offset - scenario_instance.data_offset);
							else
								map_off = tag_data_start_offset + (tag_info.data_offset - 0x3C000);
							file_stream->seekg(map_off);
							//0x3c000 is a hardcoded value in blam engine

							file_stream->read(data, tag_info.size);

							//create a meta object
							std::shared_ptr<meta> temp_meta = std::make_shared<meta>(data, tag_info.size, tag_info.data_offset, temp_plugin, file_stream, map_off, 1, *(load_tag_list.cbegin()), map_location.get_string(), tag_info.type);
							//temp_meta->Rebase_meta(0x0);
							//found unnecessary

							que_meta_list.emplace(*(load_tag_list.cbegin()), temp_meta);
							key_list.push_back(*(load_tag_list.cbegin()));

							if (recursive)
							{
								std::list<int> temp_tag_ref = temp_meta->Get_all_tag_refs();

								//to remove redundancies
								std::list<int>::iterator ref_iter = temp_tag_ref.begin();
								while (ref_iter != temp_tag_ref.end())
								{
									if (que_meta_list.find(*ref_iter) != que_meta_list.end())
										ref_iter = temp_tag_ref.erase(ref_iter);
									else
										ref_iter++;
								}

								load_tag_list.insert(load_tag_list.end(), temp_tag_ref.begin(), temp_tag_ref.end());
							}
						}
						else
						{
							//most of time this is caused due to shared stuff
							std::string temp_error = "Invalid Datum index :0x" + meta_struct::to_hex_string(*(load_tag_list.cbegin()));
							error_list.push_back(temp_error);
						}
					}
				}
				//list cleaning stuff
				load_tag_list.pop_front();
			}
			file_stream->close();

		}
		else
		{
			std::string temp_error;
			if (!custom)
				temp_error = "Couldnt open default map " + map;
			else
				temp_error = "Couldnt open custom map " + map;

			error_list.push_back(temp_error);
		}
		delete file_stream;//uh forgot that
	}
	//Return the size of the meta that is currently in the que
	unsigned int Que_meta_size()
	{
		unsigned int ret = 0;

		for (auto& i : que_meta_list)
		{
			ret += i.second->Get_Total_size();
		}

		return ret;
	}

	int Generate_SID(int table_index, int set, std::string STRING)
	{
		int l = (STRING.length() & 0xFF) << 24;
		int s = (set & 0xFF) << 16;
		int t = table_index & 0xFFFF;

		return (l | s | table_index);
	}
	///
	//Returns a list of strings along with their stringIDs
	///
	std::list<meta_struct::StringID_info> Get_SID_list(std::string map_loc)
	{
		std::list<meta_struct::StringID_info> ret;

		std::ifstream fin;
		fin.open(map_loc.c_str(), std::ios::in | std::ios::binary);

		if (!fin.is_open())
			return ret;

		int string_table_count;
		int string_index_table_offset;
		int string_table_offset;

		fin.seekg(0x170);
		fin.read((char*)&string_table_count, 4);
		fin.seekg(0x178);
		fin.read((char*)&string_index_table_offset, 4);
		fin.seekg(0x17C);
		fin.read((char*)&string_table_offset, 4);

		for (int index = 0; index < string_table_count; index++)
		{
			int table_off;
			fin.seekg(string_index_table_offset + index * 0x4);
			fin.read((char*)&table_off, 4);
			table_off = table_off & 0xFFFF;

			std::string STRING = "";

			fin.seekg(string_table_offset + table_off);
			char ch;
			do
			{
				fin.read(&ch, 1);
				STRING += ch;
			} while (ch);

			if (STRING.length() > 0)
			{
				int SID = Generate_SID(index, 0x0, STRING);//set is 0x0 cuz i couldnt figure out any other value

				meta_struct::StringID_info SIDI;

				SIDI.StringID = SID;
				SIDI.STRING = STRING;

				ret.push_back(SIDI);
			}
		}

		fin.close();
		return ret;
	}
	//sets various directories required for working of tag stuff
	void Set_directories(std::string default_maps, std::string custom_maps, std::string custom_tags, std::string plugin_loc)
	{
		maps_directory = default_maps;
		custom_maps_directory = custom_maps;
		custom_tags_directory = custom_tags;
		plugins_directory = plugin_loc;
	}
	//Updates datum_indexes and rebases tags before inserting into memory
	//pushes the tag_data in que to the tag_tables and tag_memory in the custom_tags allocated space
	void Push_Back()
	{
		if ((used_additional_meta + Que_meta_size() < _MAX_ADDITIONAL_TAG_SIZE_))
			//does some maths to help u out
			Push_Back(new_datum_index);
		else
		{
			std::string error = "Coudn't inject, Max meta size reached";
			error_list.push_back(error);
		}
	}
	//pushes the tag_data in que to the tag_tables and tag_memory at specified tag_table index
	//usually i call this to overwrite tag_table of some preloaded tag(for replacing purpose)
	void Push_Back(int datum_index)
	{
		int mem_off = def_meta_size + used_additional_meta;

		//build up inject refs
		std::list<injectRefs> my_inject_refs;

		bool replaced = false;
		for (size_t i = 0; i < key_list.size(); i++)
		{
			injectRefs temp;
			temp.old_datum = key_list[i];

			if (!replaced)
			{
				if ((datum_index & 0xFFFF) < new_datum_index)
					temp.new_datum = datum_index;
				else
					temp.new_datum = new_datum_index++;
				replaced = true;
			}
			else
			{
				temp.new_datum = new_datum_index++;
			}

			my_inject_refs.push_back(temp);
		}
		std::string temp = "Pushing back tag : " + meta_struct::to_hex_string(my_inject_refs.begin()->old_datum) + " to : " + meta_struct::to_hex_string(my_inject_refs.begin()->new_datum);
		error_list.push_back(temp);

		//StringID listing---IDC


		//update the que list tags
		for (auto& i : key_list)
		{
			que_meta_list[i]->Update_datum_indexes(my_inject_refs);
			//<stringID stuff>
			//-------------IDC anymore----------------
		}
		//Add them to the tables

		for (auto& my_inject_refs_iter : my_inject_refs)
		{
			if (def_meta_size)
			{
				injected_tag_refs.push_back(my_inject_refs_iter);
				int meta_size = que_meta_list[my_inject_refs_iter.old_datum]->Get_Total_size();

				que_meta_list[my_inject_refs_iter.old_datum]->Rebase_meta(mem_off);
				char* meta_data = que_meta_list[my_inject_refs_iter.old_datum]->Generate_meta_file();
				
				tags::tag_instance* new_instance = &tag_loader::reallocated_tag_table[my_inject_refs_iter.new_datum & 0xFFFF];

				new_instance->type = que_meta_list[my_inject_refs_iter.old_datum]->Get_type();
				new_instance->data_offset = mem_off;
				new_instance->size = meta_size;
				new_instance->datum_index = datum(my_inject_refs_iter.new_datum);

				memcpy(tags::get_tag_data() + mem_off, meta_data, meta_size);//copy to the tag memory

				//Load RAW
				Load_RAW_refs(my_inject_refs_iter.new_datum, que_meta_list[my_inject_refs_iter.old_datum]->Get_map_loc());
				//fix the global_refs
				Fix_global_objects_ref(my_inject_refs_iter.new_datum);

				delete[] meta_data;
				mem_off += meta_size;
				used_additional_meta += meta_size;
			}
			else break;
			//meta_list.emplace(my_inject_refs_iter->new_datum, que_iter->second);//add it to the meta _list				
		}
		Fix_shader_templates();
		key_list.clear();
		my_inject_refs.clear();
		que_meta_list.clear();
	}
	//clears the tags in que_list
	void Clear_que_list()
	{
		que_meta_list.clear();
	}
	//Rebases to 0x0 and dumps meta data in que in the specified tag folder
	void Dump_Que_meta()
	{
		std::ofstream fout;

		for (auto& i : que_meta_list)
		{
			std::string file_loc = custom_tags_directory + "\\que\\" + meta_struct::to_hex_string(i.first);

			i.second->Rebase_meta(0x0);
			int size = i.second->Get_Total_size();
			char* data = i.second->Generate_meta_file();
			std::string type = i.second->Get_type().string;

			file_loc += '.' + type;
			fout.open(file_loc, std::ios::out | std::ios::binary);

			fout.write(data, size);

			delete[] data;
			fout.close();
		}

	}
	//return and clears all the error messages incurred
	std::string Pop_messages()
	{
		std::string ret;
		while (!error_list.empty())
		{
			ret = error_list[error_list.size() - 1] + '\n' + ret;
			error_list.pop_back();
		}
		return ret;
	}
	//return and clears all the loaded tag list
	std::string Pop_tag_list()
	{
		std::string ret;
		while (!tag_list.empty())
		{
			ret = tag_list[tag_list.size() - 1] + '\n' + ret;
			tag_list.pop_back();
		}
		return ret;
	}
	//function to try and return a handle to the map (map_name or scenario_name(same as the actual map_name) supported)
	//Checks inside mods//maps folder first then maps folder and finally inside custom maps folder
	HANDLE try_find_map(std::string map)
	{
		std::string map_loc = map;
		//checking for full path length
		if (!PathFileExistsA(map_loc.c_str()))
		{
			if (map.find('\\') == std::string::npos)
			{
				//could be both map_name with or without extension
				if (meta_struct::Get_file_type(map) == "map")
					map_loc = mods_directory + "\\maps\\" + map;
				else
					map_loc = mods_directory + "\\maps\\" + map + ".map";

				if (PathFileExistsA(map_loc.c_str()));
				else
				{
					if (meta_struct::Get_file_type(map) == "map")
						map_loc = maps_directory + '\\' + map;
					else
						map_loc = maps_directory + '\\' + map + ".map";

					if (PathFileExistsA(map_loc.c_str()));
					else
					{
						if (meta_struct::Get_file_type(map) == "map")
							map_loc = custom_maps_directory + '\\' + map;
						else
							map_loc = custom_maps_directory + '\\' + map + ".map";
					}
				}
				return CreateFileA(map_loc.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
			}
			else
			{
				//scenario name
				//try retrieving map_name from scenario
				std::string map_name = map_loc.substr(map_loc.rfind('\\') + 1);
				map_loc = mods_directory + "\\maps\\" + map_name + ".map";
				//only tries to load from <mods_directory>\\maps cause game can auto load from default locations
				///i suggest naming map_names same as scenario names--saves the trouble
				return CreateFileA(map_loc.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
			}
		}
		else
		{
			//full path length
			return CreateFileA(map_loc.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		}
	}
	//function to load RAW_DATA of the concerned tag from meta_list
	//Carefull the tag should be loaded in the meta_tables and meta,this function just fixes its RAW_DATA
	void Load_RAW_refs(datum datum_index, std::string map_loc)
	{
		DWORD* PMapRawtableoffset = Memory::GetAddress<DWORD*>(0x4AE8B0);
		DWORD* PRawTableSize = Memory::GetAddress<DWORD*>(0x4AE8B4);

		//a little  precaution to circumvent unexpected behaviour
		DWORD oldRtable_offset = *PMapRawtableoffset;
		DWORD oldRtable_size = *PRawTableSize;

		*PMapRawtableoffset = 0x0;
		*PRawTableSize = 0x0;

		DWORD ETCOFFSET = *Memory::GetAddress<DWORD*>(0x482290);
		HANDLE old_file_handle = *Memory::GetAddress<HANDLE*>(0x4AE8A8);

		//char* ripped_map = (char*)(SharedmapBase + tag_scenario_off);

		tags::tag_instance* tag_info = &reallocated_tag_table[DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index)];
		char* tag_data = tags::get_tag_data() + reallocated_tag_table[DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index)].data_offset;

		//fail safe
		if (DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_info->datum_index) != DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index))
		{
			std::string error = "Tag: " + datum_index;
			error += " not loaded into tag tables and tag memory";
			throw new std::exception(error.c_str());
		}

		//supposing full length
		HANDLE new_file_handle = try_find_map(map_loc);
		if (new_file_handle == INVALID_HANDLE_VALUE)
		{
			//i suppose its in scenario fomat and placed in default and custom directories
			new_file_handle = global_handle_function::get_map_Handle_from_scnr(map_loc.c_str());
		}
		*Memory::GetAddress<HANDLE*>(0x4AE8A8) = new_file_handle;

		switch (tag_info->type.group)
		{
		case 'mode':

			if (*(int*)(tag_data + 0x24) > 0)
			{
				int v15 = 0;

				int off = 0;
				do
				{
					int sections_off = *(int*)(tag_data + 0x28);
					int sections_base = 0;
					if (sections_off != -1)
						sections_base = ETCOFFSET + sections_off;
					((void(__cdecl*)(int, unsigned int))Memory::GetAddress(0x2652BC))(sections_base + off + 0x38, 3u);
					++v15;
					off += 0x5C;
				} while (v15 < *(int*)(tag_data + 0x24));
			}
			break;

		case 'bitm':
		{

			int old_list_field = *Memory::GetAddress<DWORD*>(0xA49270 + 0x1FC);

			for (int i = 0; i < *(int*)(tag_data + 0x44); i++)
			{

				int bitmaps_field_off = *(int*)(tag_data + 0x48);

				int bitmaps_field_base = 0;
				if (bitmaps_field_off != -1)
					bitmaps_field_base = bitmaps_field_off + ETCOFFSET;

				int bitmaps_field = bitmaps_field_base + 0x74 * i;

				*Memory::GetAddress<DWORD*>(0xA49270 + 0x1FC) = bitmaps_field;

				int temp = 0;
				((int(__cdecl*)(int, char, int, void*))Memory::GetAddress(0x265986))(bitmaps_field, 2, 0, &temp);

				((int(__cdecl*)(int, char, int, void*))Memory::GetAddress(0x265986))(bitmaps_field, 0, 0, &temp);

			}
			*Memory::GetAddress<DWORD*>(0xA49270 + 0x1FC) = old_list_field;
			break;
		}
		case 'weat':
		{
			auto weather_tag = reinterpret_cast<c_weather_system*>(tag_data);
			for (auto i = 0; i < weather_tag->m_particle_system.count; i++)
				((void(__cdecl*)(long*, unsigned int))Memory::GetAddress(0x2652BC))(&weather_tag->m_particle_system[i]->m_geometry.block_offset, 3u);
			break;
		}
		default:
			break;
		}
		*Memory::GetAddress<HANDLE*>(0x4AE8A8) = old_file_handle;
		CloseHandle(new_file_handle);

		*PMapRawtableoffset = oldRtable_offset;
		*PRawTableSize = oldRtable_size;
	}
	void Load_RAW_refs(datum datum_index, HANDLE file)
	{
		DWORD* PMapRawtableoffset = (DWORD*)(Memory::baseAddress + 0x4AE8B0);
		DWORD* PRawTableSize = (DWORD*)(Memory::baseAddress + 0x4AE8B4);

		//a little  precaution to circumvent unexpected behaviour
		DWORD oldRtable_offset = *PMapRawtableoffset;
		DWORD oldRtable_size = *PRawTableSize;

		//hax to force loading from the disk
		*PMapRawtableoffset = 0x0;
		*PRawTableSize = 0x0;

		DWORD ETCOFFSET = *(DWORD*)(Memory::baseAddress + 0x482290);
		HANDLE old_file_handle = *(HANDLE*)(Memory::baseAddress + 0x4AE8A8);


		tags::tag_instance* tag_info = &reallocated_tag_table[DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index)];
		char* tag_data = tags::get_tag_data() + reallocated_tag_table[DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index)].data_offset;

		//fail safe
		if (DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_info->datum_index) != DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index))
		{
			std::string error = "Tag: " + datum_index;
			error += " not loaded into tag tables and tag memory";
			throw new std::exception(error.c_str());
		}

		*(HANDLE*)(Memory::baseAddress + 0x4AE8A8) = file;

		switch (tag_info->type.group)
		{
		case 'mode':

			if (*(int*)(tag_data + 0x24) > 0)
			{
				int v15 = 0;

				int off = 0;
				do
				{
					int sections_off = *(int*)(tag_data + 0x28);
					int sections_base = 0;
					if (sections_off != -1)
						sections_base = ETCOFFSET + sections_off;
					((void(__cdecl*)(int, unsigned int))Memory::GetAddress(0x2652BC))(sections_base + off + 0x38, 3u);
					++v15;
					off += 0x5C;
				} while (v15 < *(int*)(tag_data + 0x24));
			}
			break;

		case 'bitm':
		{

			int old_list_field = *Memory::GetAddress<DWORD*>(0xA49270 + 0x1FC);

			for (int i = 0; i < *(int*)(tag_data + 0x44); i++)
			{

				int bitmaps_field_off = *(int*)(tag_data + 0x48);

				int bitmaps_field_base = 0;
				if (bitmaps_field_off != -1)
					bitmaps_field_base = bitmaps_field_off + ETCOFFSET;

				int bitmaps_field = bitmaps_field_base + 0x74 * i;

				*Memory::GetAddress<DWORD*>(0xA49270 + 0x1FC) = bitmaps_field;

				int temp = 0;
				((int(__cdecl*)(int, char, int, void*))Memory::GetAddress(0x265986))(bitmaps_field, 2, 0, &temp);
				((int(__cdecl*)(int, char, int, void*))Memory::GetAddress(0x265986))(bitmaps_field, 1, 0, &temp);
				((int(__cdecl*)(int, char, int, void*))Memory::GetAddress(0x265986))(bitmaps_field, 0, 0, &temp);

			}
			*Memory::GetAddress<DWORD*>(0xA49270 + 0x1FC) = old_list_field;
			break;
		}
		case 'weat':
		{
			auto weather_tag = reinterpret_cast<c_weather_system*>(tag_data);
			for (auto i = 0; i < weather_tag->m_particle_system.count; i++)
				((void(__cdecl*)(long*, unsigned int))Memory::GetAddress(0x2652BC))(&weather_tag->m_particle_system[i]->m_geometry.block_offset, 3u);
			break;
		}
		default:
			break;
		}
		*(HANDLE*)(Memory::baseAddress + 0x4AE8A8) = old_file_handle;

		*PMapRawtableoffset = oldRtable_offset;
		*PRawTableSize = oldRtable_size;
	}
	//Fixes the reference of the tags to their global objects(vftables)
	void Fix_global_objects_ref(datum datum_index)
	{
		tag_group type = reallocated_tag_table[DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index)].type;
		datum Tdatum_index = reallocated_tag_table[DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index)].datum_index;

		if (Tdatum_index != datum_index)
		{
			std::string error = "Tag: " + datum_index;
			error += " not loaded into tag tables and tag memory";
			throw new std::exception(error.c_str());
		}

		switch (type.group)
		{
		case 'crea':
			global_objects_fix::crea_fix(DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index));
			break;
		case 'bipd':
			global_objects_fix::bipd_fix(DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index));
			break;
		case 'coll':
			global_objects_fix::coll_fix(DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index));
			break;
		case 'phmo':
			global_objects_fix::phmo_fix(DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index), false);
			break;
		case 'vehi':
			global_objects_fix::vehi_fix(DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index));
			break;
		default:
			break;
		}
	}

	void Fix_shader_templates()
	{
		typedef bool(__cdecl t_init_shader_template)(int a1);
		auto p_init_shader_template = Memory::GetAddress<t_init_shader_template*>(0x2694E6);
		tag_iterator* stem_iterator = Memory::GetAddress<tag_iterator*>(0xA4AF10);
		for (auto ref : injected_tag_refs)
		{
			auto inst = tags::get_tag_instances()[DATUM_INDEX_TO_ABSOLUTE_INDEX(ref.new_datum)];
			if (inst.type.group == _tag_group_shader_template)
			{
				//Change the iterators next tag datum index to the instance to force the next call to s_tag_data_iterator::get_next_datum() to return it.
				stem_iterator->next_tag_index = inst.datum_index;
				p_init_shader_template(1);
			}
		}
	}
		
	void Add_all_shared_refs()
	{
		tags::tag_instance* SharedTables = reinterpret_cast<tags::tag_instance*>(tags::get_tag_data() + 0x20 + 0xC * *(DWORD*)(tags::get_tag_data() + 4));

		for (int i = FIRST_SHARED_TAG_INSTANCE_INDEX; i < tags::get_tag_count(); i++)
			memcpy(&tags::get_tag_instances()[i], &SharedTables[i], sizeof(tags::tag_instance));
	}
	void Generate_sync_list(tag_group type, DWORD index)
	{
		switch (type.group)
		{
		case _tag_group_biped:
		case _tag_group_vehicle:
		case _tag_group_weapon:
		case _tag_group_garbage:
		case _tag_group_projectile:
		case _tag_group_crate:
		case _tag_group_damage_effect:
		case _tag_group_device:
		case _tag_group_scenery:
		case _tag_group_device_light_fixture:
		case _tag_group_sound_scenery:
		case _tag_group_creature:
		case _tag_group_device_machine:
		case _tag_group_equipment:
			sync_list.push_back(index);
			break;

		}
	}

	datum ResolveNewDatum(int oldDatum)
	{
		for (auto& ref : injected_tag_refs)
		{
			if (ref.old_datum == oldDatum)
				return datum(ref.new_datum);
		}
		return NONE;
	}
}

//Used to allocate somemore space for tagtables and tags
uint32 __cdecl datum_header_allocate(int32 allocation_size, int32 item_size)
{
	typedef unsigned int(_cdecl* Allocate_memory)(int size, char arg_4);
	Allocate_memory pAllocate_memory;
	pAllocate_memory = Memory::GetAddress<Allocate_memory>(0x37E69);

	//i need to allocate more space
	int modified_allocation_size = allocation_size + _MAX_ADDITIONAL_TAG_SIZE_;
	tag_loader::def_meta_size = allocation_size + 0x20;//spacing

	return INVOKE(0x37E69, 0, datum_header_allocate, modified_allocation_size, item_size);
}
//function patching to load custom tags
bool _cdecl scenario_tags_load_internal(char* scenario_path)
{
	bool result = INVOKE(0x31348, 0, scenario_tags_load_internal, scenario_path);

	//Clear the table
	for (uint32 i = _INJECTED_TAG_START_; i < tag_loader::new_datum_index; i++)
	{
		tag_loader::reallocated_tag_table[i] = tags::tag_instance{ (e_tag_group)NONE, NONE, 0, 0 };
	}
	tag_loader::que_meta_list.clear();
	tag_loader::injected_tag_refs.clear();
	tag_loader::key_list.clear();
	// reset starting_datum index
	tag_loader::used_additional_meta = 0;
	tag_loader::new_datum_index = _INJECTED_TAG_START_;


	// extending tag_tables and loading tag for all mutiplayer maps and mainmenu map
	if (cache_files_get_header()->type != scenario_type_singleplayer_shared)
	{
		// Grab the current 
		uint32* tag_table_start = Memory::GetAddress<uint32*>(0x47CD50);
		memset((BYTE*)tag_loader::reallocated_tag_table, 0, 0x3BA40);

		if (*tag_table_start != NULL)
		{
			memcpy(tag_loader::reallocated_tag_table, (BYTE*)*tag_table_start, 0x3BA40);
			*tag_table_start = (uint32)tag_loader::reallocated_tag_table;
		}
	}

	return result;
}
void _Patch_calls()
{
	//Todo :: Update Offsets for Dedi
	PatchCall(Memory::GetAddress(0x313B2), datum_header_allocate);//allocating more space for meta loading
	PatchCall(Memory::GetAddress(0x3166B), scenario_tags_load_internal);//default maps meta loading
	PatchCall(Memory::GetAddress(0x315ED), scenario_tags_load_internal);//custom maps meta loading,i know i am taking risks	

	//client side desync fix
	///(noping out jump instructions)	
	NopFill(Memory::GetAddress(0x316CE), 2);
	NopFill(Memory::GetAddress(0x316DC), 2);
}
void Initialise_tag_loader()
{
	tag_loader::reallocated_tag_table = new tags::tag_instance[_MAX_TAG_TABLE_SIZE_];

	std::string game_dir(GetExeDirectoryNarrow());
	std::string def_maps_loc = game_dir + "\\maps";


	tag_loader::mods_directory = game_dir + "\\mods";
	tag_loader::Set_directories(def_maps_loc, "", tag_loader::mods_directory + "\\tags", tag_loader::mods_directory + "\\plugins");//no custom support yet
	_Patch_calls();
}