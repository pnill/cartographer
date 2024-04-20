#include "stdafx.h"
#include "tag_loader.h"

#include "bitmaps/bitmap_group.h"
#include "cache/cache_files.h"
#include "game/game_options.h"
#include "scenario/scenario.h"
#include "render/weather_definitions.h"

#include "Blam/Cache/TagGroups/scenery_definition.hpp"

#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "models/models.h"
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
namespace tag_post_process_functions
{
	void __cdecl biped_init_havok(datum datum_index)
	{
		void (_cdecl *sub_EC23F)(datum) = (void(_cdecl*)(datum))Memory::GetAddress(0x1389B0);
		sub_EC23F(datum_index);
	}
	void __cdecl creature_init_havok(datum datum_index)
	{
		void (_cdecl *sub_EC23F)(datum) = (void(_cdecl*)(datum))Memory::GetAddress(0x138985);
		sub_EC23F(datum_index);
	}
	void __cdecl vehicle_init_havok(datum datum_index)
	{
		void(_cdecl *sub_EC23F)(datum) = (void(_cdecl*)(datum))Memory::GetAddress(0x13895A);
		sub_EC23F(datum_index);
	}
	void __cdecl collision_init_havok(datum datum_index)
	{
		void(_cdecl *sub_EC23F)(datum) = (void(_cdecl*)(datum))Memory::GetAddress(0x7BE5C);
		sub_EC23F(datum_index);
	}
	void __cdecl physics_model_init_havok(datum datum_index, bool unk)
	{
		void(_cdecl *sub_EC23F)(datum, bool) = (void(_cdecl*)(datum, bool))Memory::GetAddress(0x7B844);
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
	std::map<int, std::shared_ptr<meta>> loaded_tag_data;//<datum_index,meta_junk>contains the list of tags that are currently in que to loaded in memory
	std::vector<int> loaded_tag_data_order;//another var just to keep the keys along with the correct order
	std::vector<int> injected_list;
	//map<int, shared_ptr<meta>> meta_list;//<datum_index,meta_junk>contains the the list of tags that are currently loaded in memory(never gonna use it anyway)
	std::vector<std::string> error_list;//contains various messages generated during various processes,shouldnt had named it error list
	std::vector<std::string> tag_list;//contains a list of tag_indices along with their names(currently implemented only for module loading)
	std::list<loaded_tag_datum_mapping> injected_tag_reference_map;
	unsigned int base_map_tag_size = 0;
	unsigned int used_additional_meta = 0;
	unsigned int next_available_datum_index = _INJECTED_TAG_START_;//first datum index
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

			tags::tag_instance current_tag_instance;

			//create a tag_list to load
			std::list<datum> tag_datums_to_load;
			tag_datums_to_load.push_back(datum_index);

			//----------------LOOPING STUFF
			while (tag_datums_to_load.size())
			{
				// grab the first datum from the list
				datum current_datum = *tag_datums_to_load.cbegin();

				if (current_datum != NONE && current_datum != 0)
				{

					// grab the instance from the tags header
					file_stream->seekg(instance_table_offset + (0xFFFF & current_datum) * sizeof(tags::tag_instance));
					file_stream->read((char*)&current_tag_instance, sizeof(tags::tag_instance));

					if (current_datum == current_tag_instance.datum_index && current_tag_instance.type.group != _tag_group_sound)
					{
						tag_group group_name = tag_group_get_name(current_tag_instance.type);

						char null_terminated_string[5] = { group_name.string[0], group_name.string[1], group_name.string[2], group_name.string[3], '\0' };
						std::shared_ptr<plugins_field> temp_plugin = Get_plugin(null_terminated_string);

						//precaution for plugin load errors
						if (!temp_plugin)
							break;

						//we first check the integrity of the datum_index
						if (current_tag_instance.datum_index == current_datum && current_tag_instance.data_offset && current_tag_instance.size > 0 && (loaded_tag_data.find(current_tag_instance.datum_index) == loaded_tag_data.cend()))
						{
							// allocate a buffer to store the loaded tags data
							char* tag_data = new char[current_tag_instance.size];

							int tag_data_offset;

							// check if the tag is being loaded from a shared cache type file
							if (!Check_shared(file_stream))
								tag_data_offset = tag_data_start_offset + (current_tag_instance.data_offset - scenario_instance.data_offset);
							else
								tag_data_offset = tag_data_start_offset + (current_tag_instance.data_offset - 0x3C000);

							// write the tag data to the buffer
							file_stream->seekg(tag_data_offset);
							file_stream->read(tag_data, current_tag_instance.size);

							//create a meta object
							std::shared_ptr<meta> temp_meta = std::make_shared<meta>(tag_data, current_tag_instance.size, current_tag_instance.data_offset, temp_plugin, file_stream, tag_data_offset, 1, current_datum, map_location.get_string(), current_tag_instance.type);

							loaded_tag_data.emplace(current_datum, temp_meta);
							loaded_tag_data_order.push_back(current_datum);

							// todo: redo this
							if (recursive)
							{
								std::list<int> temp_tag_ref = temp_meta->Get_all_tag_refs();

								//to remove redundancies
								std::list<int>::iterator ref_iter = temp_tag_ref.begin();
								while (ref_iter != temp_tag_ref.end())
								{
									if (loaded_tag_data.find(*ref_iter) != loaded_tag_data.end())
										ref_iter = temp_tag_ref.erase(ref_iter);
									else
										ref_iter++;
								}

								tag_datums_to_load.insert(tag_datums_to_load.end(), temp_tag_ref.begin(), temp_tag_ref.end());
							}
						}
						else
						{
							//most of time this is caused due to shared stuff
							std::string temp_error = "Invalid Datum index :0x" + meta_struct::to_hex_string(*(tag_datums_to_load.cbegin()));
							error_list.push_back(temp_error);
						}
					}
				}
				//list cleaning stuff
				tag_datums_to_load.pop_front();
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

		for (auto& i : loaded_tag_data)
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
			Push_Back(next_available_datum_index);
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
		// offset from tag_data_start the injected tag_data will be placed
		uint32 injected_tag_data_offset = base_map_tag_size + used_additional_meta;

		// create a map of cache_index to injected_index
		std::list<loaded_tag_datum_mapping> t_injected_tag_datum_map;

		bool replaced = false;
		for (datum i : loaded_tag_data_order)
		{
			loaded_tag_datum_mapping loaded_tag_datum_map;
			loaded_tag_datum_map.cache_index = i;

			if (!replaced)
			{
				if ((datum_index & 0xFFFF) < next_available_datum_index)
					loaded_tag_datum_map.injected_index = datum_index;
				else
					loaded_tag_datum_map.injected_index = next_available_datum_index++;
				replaced = true;
			}
			else
			{
				loaded_tag_datum_map.injected_index = next_available_datum_index++;
			}

			t_injected_tag_datum_map.push_back(loaded_tag_datum_map);
		}


		//StringID listing---IDC


		// update the tag_references inside the loaded tag data
		for (auto& i : loaded_tag_data_order)
		{
			loaded_tag_data[i]->Update_datum_indexes(t_injected_tag_datum_map);
			//<stringID stuff>
			//-------------IDC anymore----------------
		}

		//Add them to the tables

		for (auto& current_tag_datum_map : t_injected_tag_datum_map)
		{
			if (base_map_tag_size)
			{
				injected_tag_reference_map.push_back(current_tag_datum_map);

				std::shared_ptr<meta> loaded_tag = loaded_tag_data[current_tag_datum_map.cache_index];
				int total_tag_size = loaded_tag->Get_Total_size();

				loaded_tag->Rebase_meta(injected_tag_data_offset);
				char* tag_data = loaded_tag->Generate_meta_file();
				
				tags::tag_instance* new_instance = &tag_loader::reallocated_tag_table[current_tag_datum_map.injected_index & 0xFFFF];

				new_instance->type = loaded_tag->Get_type();
				new_instance->data_offset = injected_tag_data_offset;
				new_instance->size = total_tag_size;
				new_instance->datum_index = current_tag_datum_map.injected_index;

				memcpy(tags::get_tag_data() + injected_tag_data_offset, tag_data, total_tag_size);//copy to the tag memory

				load_raw_table_data(current_tag_datum_map.injected_index, loaded_tag->Get_map_loc());
				setup_injected_tag_havok_vtables(current_tag_datum_map.injected_index);

				delete[] tag_data;
				injected_tag_data_offset += total_tag_size;
				used_additional_meta += total_tag_size;
			}
			else break;			
		}
		initialize_injected_shader_templates();
		loaded_tag_data_order.clear();
		t_injected_tag_datum_map.clear();
		loaded_tag_data.clear();
	}
	//clears the tags in que_list
	void Clear_que_list()
	{
		loaded_tag_data.clear();
	}

	// Commented out as it's unused but keeping as an example for future use
	//Rebases to 0x0 and dumps meta data in que in the specified tag folder
	//void Dump_Que_meta()
	//{
	//	std::ofstream fout;

	//	for (auto& i : loaded_tag_data)
	//	{
	//		std::string file_loc = custom_tags_directory + "\\que\\" + meta_struct::to_hex_string(i.first);

	//		i.second->Rebase_meta(0x0);
	//		int size = i.second->Get_Total_size();
	//		char* data = i.second->Generate_meta_file();
	//		std::string type = i.second->Get_type().string;

	//		file_loc += '.' + type;
	//		fout.open(file_loc, std::ios::out | std::ios::binary);

	//		fout.write(data, size);

	//		delete[] data;
	//		fout.close();
	//	}
	//}

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


	//function to try and return a handle to the map (map_name or scenario_name(same as the actual map_name) supported)
	//Checks inside mods//maps folder first then maps folder and finally inside custom maps folder
	HANDLE create_file_handle(c_static_string260* map_file)
	{
		return CreateFileA(map_file->get_string(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	}
	//function to load RAW_DATA of the concerned tag from meta_list
	//Carefull the tag should be loaded in the meta_tables and meta,this function just fixes its RAW_DATA
	void load_raw_table_data(datum datum_index, c_static_string260* map_loc)
	{
		DWORD* PMapRawtableoffset = Memory::GetAddress<DWORD*>(0x4AE8B0);
		DWORD* PRawTableSize = Memory::GetAddress<DWORD*>(0x4AE8B4);

		//a little  precaution to circumvent unexpected behaviour
		DWORD oldRtable_offset = *PMapRawtableoffset;
		DWORD oldRtable_size = *PRawTableSize;

		*PMapRawtableoffset = 0x0;
		*PRawTableSize = 0x0;

		HANDLE old_file_handle = *Memory::GetAddress<HANDLE*>(0x4AE8A8);

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
		HANDLE new_file_handle = create_file_handle(map_loc);
		if (new_file_handle == INVALID_HANDLE_VALUE)
		{
			//i suppose its in scenario fomat and placed in default and custom directories
			new_file_handle = global_handle_function::get_map_Handle_from_scnr(map_loc->get_string());
		}
		*Memory::GetAddress<HANDLE*>(0x4AE8A8) = new_file_handle;

		switch (tag_info->type.group)
		{
		case 'mode':
		{
			render_model_definition* model_definition = (render_model_definition*)tag_data;
			if (model_definition->sections.count > 0)
			{
				int current_section_index = 0;
				do
				{
					render_model_section* model_section = model_definition->sections[current_section_index];

					((void(__cdecl*)(geometry_block_info*, unsigned int))Memory::GetAddress(0x2652BC))(&model_section->geometry_block_info, 3u);

					++current_section_index;
				} while (current_section_index < model_definition->sections.count);
			}
			break;
		}

		case 'bitm':
		{

			int old_list_field = *Memory::GetAddress<DWORD*>(0xA49270 + 0x1FC);
			bitmap_group* bitmap_definition = (bitmap_group*)tag_data;

			for (int i = 0; i < bitmap_definition->bitmaps.count; i++)
			{
				bitmap_data* bitmap_item = bitmap_definition->bitmaps[i];

				*Memory::GetAddress<bitmap_data**>(0xA49270 + 0x1FC) = bitmap_item;

				int temp = 0;
				((int(__cdecl*)(bitmap_data*, char, int, void*))Memory::GetAddress(0x265986))(bitmap_item, 2, 0, &temp);

				((int(__cdecl*)(bitmap_data*, char, int, void*))Memory::GetAddress(0x265986))(bitmap_item, 0, 0, &temp);

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


	//Fixes the reference of the tags to their global objects(vftables)
	void setup_injected_tag_havok_vtables(datum datum_index)
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
		case _tag_group_creature:
			tag_post_process_functions::creature_init_havok(DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index));
			break;
		case _tag_group_biped:
			tag_post_process_functions::biped_init_havok(DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index));
			break;
		case _tag_group_collision_model:
			tag_post_process_functions::collision_init_havok(DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index));
			break;
		case _tag_group_physics_model:
			tag_post_process_functions::physics_model_init_havok(DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index), false);
			break;
		case _tag_group_vehicle:
			tag_post_process_functions::vehicle_init_havok(DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index));
			break;
		default:
			break;
		}
	}

	void initialize_injected_shader_templates()
	{
		typedef bool(__cdecl t_init_shader_template)(int a1);
		auto p_init_shader_template = Memory::GetAddress<t_init_shader_template*>(0x2694E6);
		tag_iterator* stem_iterator = Memory::GetAddress<tag_iterator*>(0xA4AF10);
		for (auto ref : injected_tag_reference_map)
		{
			auto inst = tags::get_tag_instances()[DATUM_INDEX_TO_ABSOLUTE_INDEX(ref.injected_index)];
			if (inst.type.group == _tag_group_shader_template)
			{
				//Change the iterators next tag datum index to the instance to force the next call to s_tag_data_iterator::get_next_datum() to return it.
				stem_iterator->next_tag_index = inst.datum_index;
				p_init_shader_template(1);
			}
		}
	}

	// not needed but left for any obscure future use. Forge?
	//void Add_all_shared_refs()
	//{
	//	tags::tag_instance* SharedTables = reinterpret_cast<tags::tag_instance*>(tags::get_tag_data() + 0x20 + 0xC * *(DWORD*)(tags::get_tag_data() + 4));

	//	for (int i = FIRST_SHARED_TAG_INSTANCE_INDEX; i < tags::get_tag_count(); i++)
	//		memcpy(&tags::get_tag_instances()[i], &SharedTables[i], sizeof(tags::tag_instance));
	//}

	datum resolve_cache_index_to_injected(int oldDatum)
	{
		for (auto& ref : injected_tag_reference_map)
		{
			if (ref.cache_index == oldDatum)
				return datum(ref.injected_index);
		}
		return NONE;
	}
}

//Used to allocate some more space for tag tables and tags
uint32 __cdecl datum_header_allocate(int32 allocation_size, int32 item_size)
{
	typedef unsigned int(_cdecl* Allocate_memory)(int size, char arg_4);
	Allocate_memory pAllocate_memory;
	pAllocate_memory = Memory::GetAddress<Allocate_memory>(0x37E69);

	//i need to allocate more space
	int modified_allocation_size = allocation_size + _MAX_ADDITIONAL_TAG_SIZE_;
	tag_loader::base_map_tag_size = allocation_size + 0x20;//spacing

	return INVOKE(0x37E69, 0, datum_header_allocate, modified_allocation_size, item_size);
}
//function patching to load custom tags
bool _cdecl scenario_tags_load_internal(char* scenario_path)
{
	bool result = INVOKE(0x31348, 0, scenario_tags_load_internal, scenario_path);

	//Clear the table
	for (uint32 i = _INJECTED_TAG_START_; i < tag_loader::next_available_datum_index; i++)
	{
		tag_loader::reallocated_tag_table[i] = tags::tag_instance{ (e_tag_group)NONE, NONE, 0, 0 };
	}
	tag_loader::loaded_tag_data.clear();
	tag_loader::injected_tag_reference_map.clear();
	tag_loader::loaded_tag_data_order.clear();
	// reset starting_datum index
	tag_loader::used_additional_meta = 0;
	tag_loader::next_available_datum_index = _INJECTED_TAG_START_;


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