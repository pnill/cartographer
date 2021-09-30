
#include "tag_loader.h"
#include "..\Util\filesys.h"

#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"

#include "Blam\Common\Common.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "Blam/Cache/TagGroups/scenery_definition.hpp"
#include "Blam/Cache/TagGroups/model_defenition.hpp"

#include "Util/Hooks/Hook.h"

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
	std::string def_maps_dir;
	std::string cus_maps_dir;
	std::string cus_tag_dir;
	std::string plugins_dir;
	std::string mods_dir;

	static std::map<std::string, std::shared_ptr<plugins_field>> plugins_list;//contains list of various plugin structures
	std::map<int, std::shared_ptr<meta>> que_meta_list;//<datum_index,meta_junk>contains the list of tags that are currently in que to loaded in memory
	std::vector<int> key_list;//another var just to keep the keys along with the correct order
	//map<int, shared_ptr<meta>> meta_list;//<datum_index,meta_junk>contains the the list of tags that are currently loaded in memory(never gonna use it anyway)
	std::vector<std::string> error_list;//contains various messages generated during various processes,shouldnt had named it error list
	std::vector<std::string> tag_list;//contains a list of tag_indices along with their names(currently implemented only for module loading)
	std::list<injectRefs> injected_tag_refs;
	unsigned int def_meta_size = 0;
	unsigned int ext_meta_size = 0;
	unsigned int new_datum_index = _INJECTED_TAG_START_;//first datum index
	tags::tag_instance* new_Tables;//new tables pointer

	std::string meta_list_target_map;//name of the map for which the meta list has been target(used for StringIDs)
	std::string que_list_target_map;//name of the map from which the que list has been loaded(used for StringID retargeting)
	std::vector<DWORD> sync_list;//container to store tag indices of sync-able Tags

	std::shared_ptr<plugins_field> Get_plugin(std::string type)
	{
		//we first look into the already loaded plugin list
		for (auto& i : plugins_list)
		{
			if (i.first == type)
				return i.second;
		}
		//it doesnt contain it therfore we need to load the plugin
		std::string plugin_loc = plugins_dir + '\\' + type + ".xml";
		std::shared_ptr<plugins_field> temp_plugin = meta_struct::Get_Tag_stucture_from_plugin(plugin_loc);

		if (temp_plugin)
			plugins_list.emplace(type, temp_plugin);
		else
		{
			std::string error = "Couldnt load plugin " + type + ".xml";
			error_list.push_back(error);
		}

		return temp_plugin;
	}
	//returns whether the map is a shared map or not
	bool Check_shared(std::ifstream* fin)
	{
		char* map_header = new char[0x800];
		fin->seekg(0x0);
		fin->read(map_header, 0x800);

		if (tags::get_cache_header()->type == e_engine_type::MultiplayerShared || tags::get_cache_header()->type == e_engine_type::SinglePlayerShared)
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
			map_loc = mods_dir + "\\maps\\" + map;
		else
			map_loc = mods_dir + "\\maps\\" + map + ".map";

		if (PathFileExistsA(map_loc.c_str()))
		{
			return true;
		}
		if (meta_struct::Get_file_type(map) == "map")
			map_loc = def_maps_dir + '\\' + map;
		else
			map_loc = def_maps_dir + '\\' + map + ".map";

		if (PathFileExistsA(map_loc.c_str()))
		{
			return true;
		}

		if (meta_struct::Get_file_type(map) == "map")
			map_loc = cus_maps_dir + '\\' + map;
		else
			map_loc = cus_maps_dir + '\\' + map + ".map";

		return PathFileExistsA(map_loc.c_str());

	}

	datum Get_tag_datum(std::string tag_name, blam_tag type, std::string map)
	{
		std::ifstream* fin;
		std::string map_loc;

		//logic to check the existence of the map at subsequent directories
		///mods->default->custom
		if (meta_struct::Get_file_type(map) == "map")
			map_loc = mods_dir + "\\maps\\" + map;
		else
			map_loc = mods_dir + "\\maps\\" + map + ".map";

		if (PathFileExistsA(map_loc.c_str()));
		else
		{
			if (meta_struct::Get_file_type(map) == "map")
				map_loc = def_maps_dir + '\\' + map;
			else
				map_loc = def_maps_dir + '\\' + map + ".map";

			if (PathFileExistsA(map_loc.c_str()));
			else
			{
				if (meta_struct::Get_file_type(map) == "map")
					map_loc = cus_maps_dir + '\\' + map;
				else
					map_loc = cus_maps_dir + '\\' + map + ".map";
			}
		}

		fin = new std::ifstream(map_loc.c_str(), std::ios::binary | std::ios::in);

		int tag_index = -1;
		bool found_index = false;

		if (fin->is_open())
		{
			fin->seekg(716);
			int tag_count, file_table_offset = 0;
			fin->read((char*)&tag_count, 4);
			fin->read((char*)&file_table_offset, 4);

			int table_off, table_size = 0;

			fin->seekg(0x10);
			fin->read((char*)&table_off, 4);
			fin->read((char*)&table_size, 4);

			fin->seekg(table_off + 4);
			int temp;
			fin->read((char*)&temp, 4);

			int table_start = table_off + 0xC * temp + 0x20;

			tags::tag_instance tag_info;

			fin->seekg(file_table_offset);
			int cpos = 0;
			char ch = ' ';
			std::string input;
			for (auto i = 0; i < tag_count; i++)
			{
				std::getline(*fin, input, '\0');
				if (input == tag_name)
				{
					cpos = fin->tellg();
					fin->seekg(table_start + i * sizeof(tags::tag_instance));
					fin->read((char*)&tag_info, sizeof(tags::tag_instance));
					if (tag_info.type == type)
					{
						delete fin;
						return tag_info.datum_index;
					}
					fin->seekg(cpos);
				}
			}
		}

		/*if(found_index)
		{
			int table_off, table_size = 0;

			fin->seekg(0x10);
			fin->read((char*)&table_off, 4);
			fin->read((char*)&table_size, 4);

			fin->seekg(table_off + 4);
			int temp;
			fin->read((char*)&temp, 4);

			int table_start = table_off + 0xC * temp + 0x20;

			tags::tag_instance tag_info;
			fin->seekg(table_start + tag_index * sizeof(tags::tag_instance));

			fin->read((char*)&tag_info, sizeof(tags::tag_instance));
			fin->close();
			delete fin;
			return tag_info.datum_index;
		}*/
		fin->close();
		delete fin;
		return DATUM_NONE;
	}
	//Loads a tag from specified map in accordance with the datum index supplied
	///custom flag is no more needed
	void Load_tag(int datum_index, bool recursive, std::string map, bool custom)
	{
		std::ifstream* fin;

		que_list_target_map = map;
		std::string map_loc;

		//logic to check the existence of the map at subsequent directories
		///mods->default->custom
		if (meta_struct::Get_file_type(map) == "map")
			map_loc = mods_dir + "\\maps\\" + map;
		else
			map_loc = mods_dir + "\\maps\\" + map + ".map";

		if (PathFileExistsA(map_loc.c_str()));
		else
		{
			if (meta_struct::Get_file_type(map) == "map")
				map_loc = def_maps_dir + '\\' + map;
			else
				map_loc = def_maps_dir + '\\' + map + ".map";

			if (PathFileExistsA(map_loc.c_str()));
			else
			{
				if (meta_struct::Get_file_type(map) == "map")
					map_loc = cus_maps_dir + '\\' + map;
				else
					map_loc = cus_maps_dir + '\\' + map + ".map";
			}
		}

		fin = new std::ifstream(map_loc.c_str(), std::ios::binary | std::ios::in);

		if (fin->is_open())
		{
			std::string temp_string = "Loading tag : " + meta_struct::to_hex_string(datum_index) + " from " + map;
			error_list.push_back(temp_string);

			//some meta reading prologue
			int table_off, table_size = 0;

			fin->seekg(0x10);
			fin->read((char*)&table_off, 4);
			fin->read((char*)&table_size, 4);


			fin->seekg(table_off + 4);
			int temp;
			fin->read((char*)&temp, 4);

			int table_start = table_off + 0xC * temp + 0x20;
			int scnr_off = table_off + table_size;

			int scnr_memaddr;
			fin->seekg(table_start + 0x8);
			fin->read((char*)&scnr_memaddr, 4);

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
					fin->seekg(table_start + (0xFFFF & *(load_tag_list.cbegin())) * sizeof(tags::tag_instance));

					fin->read((char*)&tag_info, sizeof(tags::tag_instance));

					if (*(load_tag_list.cbegin()) == tag_info.datum_index)
					{
						std::shared_ptr<plugins_field> temp_plugin = Get_plugin(tag_info.type.as_string());

						//precaution for plugin load errors
						if (!temp_plugin)
							break;

						//we first check the integrity of the datum_index
						if (tag_info.datum_index == *(load_tag_list.cbegin()) && tag_info.data_offset && tag_info.size > 0 && (que_meta_list.find(tag_info.datum_index) == que_meta_list.cend()))
						{
							//read the meta data from the map            
							char* data = new char[tag_info.size];

							int map_off;
							if (!Check_shared(fin))
								map_off = scnr_off + (tag_info.data_offset - scnr_memaddr);
							else
								map_off = scnr_off + (tag_info.data_offset - 0x3C000);
							fin->seekg(map_off);
							//0x3c000 is a hardcoded value in blam engine

							fin->read(data, tag_info.size);

							//create a meta object
							std::shared_ptr<meta> temp_meta = std::make_shared<meta>(data, tag_info.size, tag_info.data_offset, temp_plugin, fin, map_off, 1, *(load_tag_list.cbegin()), map_loc, tag_info.type);
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
			fin->close();

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
		delete fin;//uh forgot that
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
		def_maps_dir = default_maps;
		cus_maps_dir = custom_maps;
		cus_tag_dir = custom_tags;
		plugins_dir = plugin_loc;
	}
	//Updates datum_indexes and rebases tags before inserting into memory
	//pushes the tag_data in que to the tag_tables and tag_memory in the custom_tags allocated space
	void Push_Back()
	{
		if ((ext_meta_size + Que_meta_size() < _MAX_ADDITIONAL_TAG_SIZE_))
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
		int mem_off = def_meta_size + ext_meta_size;

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
		injected_tag_refs = my_inject_refs;
		for (auto& my_inject_refs_iter : my_inject_refs)
		{
			if (def_meta_size)
			{
				int meta_size = que_meta_list[my_inject_refs_iter.old_datum]->Get_Total_size();
				tags::tag_instance tables_data;

				que_meta_list[my_inject_refs_iter.old_datum]->Rebase_meta(mem_off);
				char* meta_data = que_meta_list[my_inject_refs_iter.old_datum]->Generate_meta_file();

				//blam_tag type(blam_tag::tag_group_type(std::atoi(.c_str())));

				tables_data.type = que_meta_list[my_inject_refs_iter.old_datum]->Get_type();
				tables_data.data_offset = mem_off;
				tables_data.size = meta_size;
				tables_data.datum_index = datum(my_inject_refs_iter.new_datum);
				tags::tag_instance* temp_write_off = &tag_loader::new_Tables[my_inject_refs_iter.new_datum & 0xFFFF];
				memcpy(temp_write_off, &tables_data, sizeof(tags::tag_instance));//copy to the tables

				memcpy(tags::get_tag_data() + mem_off, meta_data, meta_size);//copy to the tag memory

				//Load RAW
				Load_RAW_refs(my_inject_refs_iter.new_datum, que_meta_list[my_inject_refs_iter.old_datum]->Get_map_loc());
				//fix the global_refs
				Fix_global_objects_ref(my_inject_refs_iter.new_datum);

				delete[] meta_data;
				mem_off += meta_size;
				ext_meta_size += meta_size;
			}
			else break;
			//meta_list.emplace(my_inject_refs_iter->new_datum, que_iter->second);//add it to the meta _list				
		}
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
			std::string file_loc = cus_tag_dir + "\\que\\" + meta_struct::to_hex_string(i.first);

			i.second->Rebase_meta(0x0);
			int size = i.second->Get_Total_size();
			char* data = i.second->Generate_meta_file();
			std::string type = i.second->Get_type().as_string();

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
					map_loc = mods_dir + "\\maps\\" + map;
				else
					map_loc = mods_dir + "\\maps\\" + map + ".map";

				if (PathFileExistsA(map_loc.c_str()));
				else
				{
					if (meta_struct::Get_file_type(map) == "map")
						map_loc = def_maps_dir + '\\' + map;
					else
						map_loc = def_maps_dir + '\\' + map + ".map";

					if (PathFileExistsA(map_loc.c_str()));
					else
					{
						if (meta_struct::Get_file_type(map) == "map")
							map_loc = cus_maps_dir + '\\' + map;
						else
							map_loc = cus_maps_dir + '\\' + map + ".map";
					}
				}
				return CreateFileA(map_loc.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
			}
			else
			{
				//scenario name
				//try retrieving map_name from scenario
				std::string map_name = map_loc.substr(map_loc.rfind('\\') + 1);
				map_loc = mods_dir + "\\maps\\" + map_name + ".map";
				//only tries to load from <mods_dir>\\maps cause game can auto load from default locations
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

		tags::tag_instance* tag_info = &new_Tables[DATUM_ABSOLUTE_INDEX(datum_index)];
		char* tag_data = tags::get_tag_data() + new_Tables[DATUM_ABSOLUTE_INDEX(datum_index)].data_offset;

		//fail safe
		if (DATUM_ABSOLUTE_INDEX(tag_info->datum_index) != DATUM_ABSOLUTE_INDEX(datum_index))
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

		switch (tag_info->type.as_int())
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


		tags::tag_instance* tag_info = &new_Tables[DATUM_ABSOLUTE_INDEX(datum_index)];
		char* tag_data = tags::get_tag_data() + new_Tables[DATUM_ABSOLUTE_INDEX(datum_index)].data_offset;

		//fail safe
		if (DATUM_ABSOLUTE_INDEX(tag_info->datum_index) != DATUM_ABSOLUTE_INDEX(datum_index))
		{
			std::string error = "Tag: " + datum_index;
			error += " not loaded into tag tables and tag memory";
			throw new std::exception(error.c_str());
		}

		*(HANDLE*)(Memory::baseAddress + 0x4AE8A8) = file;

		switch (tag_info->type.as_int())
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
		blam_tag type = new_Tables[DATUM_ABSOLUTE_INDEX(datum_index)].type;
		datum Tdatum_index = new_Tables[DATUM_ABSOLUTE_INDEX(datum_index)].datum_index;

		if (Tdatum_index != datum_index)
		{
			std::string error = "Tag: " + datum_index;
			error += " not loaded into tag tables and tag memory";
			throw new std::exception(error.c_str());
		}

		switch (type.as_int())
		{
		case 'crea':
			global_objects_fix::crea_fix(DATUM_ABSOLUTE_INDEX(datum_index));
			break;
		case 'bipd':
			global_objects_fix::bipd_fix(DATUM_ABSOLUTE_INDEX(datum_index));
			break;
		case 'coll':
			global_objects_fix::coll_fix(DATUM_ABSOLUTE_INDEX(datum_index));
			break;
		case 'phmo':
			global_objects_fix::phmo_fix(DATUM_ABSOLUTE_INDEX(datum_index), false);
			break;
		case 'vehi':
			global_objects_fix::vehi_fix(DATUM_ABSOLUTE_INDEX(datum_index));
			break;
		default:
			break;
		}
	}
	//Basically created to easily transfer working tags from system to system
	int Load_tag_module(std::string loc)
	{
		int ret = -1;

		if (!new_Tables)
			return 0;

		cache_loader* my_loader = new cache_loader(loc);

		if (my_loader->get_last_error() != UNABLE_TO_LOCATE_FILE)
		{
			HANDLE file_handle = CreateFileA(loc.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

			cache_BLOCK* module_tag_table = my_loader->get_BLOCK("tag_table");
			cache_BLOCK* module_tag_data = my_loader->get_BLOCK("tag_data");
			cache_BLOCK* module_tag_maps = my_loader->get_BLOCK("tag_maps");
			cache_BLOCK* module_tag_names = my_loader->get_BLOCK("tag_names");

			if (ext_meta_size + module_tag_data->size > _MAX_ADDITIONAL_TAG_SIZE_)
			{
				std::string error = "Couldnt load module, MAX_ADDITIONAL_TAG_SIZE_ reached";
				error_list.push_back(error);
				delete my_loader;

				return ret;
			}
			ret = new_datum_index;
			std::string error = "Loading module: " + loc + " to datum_index: " + meta_struct::to_hex_string(new_datum_index);
			error_list.push_back(error);

			int mem_off = def_meta_size + ext_meta_size;

			tags::tag_instance* t_ptr = &new_Tables[new_datum_index];
			char* d_ptr = tags::get_tag_data() + mem_off;
			char* m_ptr = module_tag_maps->data;
			char* n_ptr = module_tag_names->data;
			if (n_ptr == nullptr)
				throw new std::exception("tag_names block not found");

			//copying tables
			memcpy(t_ptr, module_tag_table->data, module_tag_table->size);
			memcpy(d_ptr, module_tag_data->data, module_tag_data->size);

			std::list<injectRefs> my_inject_refs;
			std::vector<std::shared_ptr<meta>> module_tags;

			int tags_first_d_index = (&t_ptr[0])->datum_index;

			for (int i = 0; i < module_tag_table->size / sizeof(tags::tag_instance); i++)
			{
				injectRefs t_ref;

				int size = (&t_ptr[i])->size;
				int mem_off = (&t_ptr[i])->data_offset;
				(&t_ptr[i])->datum_index = new_datum_index; //update the datum_index in the tag_tables


				t_ref.old_datum = tags_first_d_index++;
				t_ref.new_datum = new_datum_index++;//assign and increment it for next tag

				std::shared_ptr<plugins_field> t_plugin = Get_plugin((&t_ptr[i])->type.as_string());
				std::shared_ptr<meta> t_meta = std::make_shared<meta>(d_ptr, size, mem_off, t_plugin, 1, t_ref.old_datum);

				my_inject_refs.push_back(t_ref);
				module_tags.push_back(t_meta);

				std::string t_name = n_ptr;
				tag_list.push_back(t_name.substr(t_name.rfind('\\') + 1) + ",0x" + meta_struct::to_hex_string(t_ref.new_datum));
				tag_loader::Generate_sync_list((&t_ptr[i])->type.as_int(), t_ref.new_datum);

				n_ptr += t_name.size() + 1;
				d_ptr += size;
			}
			for (size_t i = 0; i < module_tags.size(); i++)
			{
				int new_mem_off = def_meta_size + ext_meta_size;

				(&t_ptr[i])->data_offset = new_mem_off;

				module_tags[i]->Update_datum_indexes(my_inject_refs);
				module_tags[i]->Rebase_meta(new_mem_off);

				Load_RAW_refs((&t_ptr[i])->datum_index, file_handle);
				Fix_global_objects_ref((&t_ptr[i])->datum_index);

				//as the tags are from the same map
				//m_ptr += strlen(m_ptr) + 1;
				ext_meta_size += module_tags[i]->Get_Total_size();
			}
			module_tags.clear();
			CloseHandle(file_handle);

		}
		else
		{
			std::string error = "Couldnt find module: " + loc;
			error_list.push_back(error);
		}
		delete my_loader;

		return ret;
	}
	//Simiplify the injection process
	void Parse_query_file(std::string loc)
	{
		query_parser* my_parser = new query_parser(loc);
		addDebugText(my_parser->_getlogs().c_str());
		delete my_parser;
	}
	void Add_all_shared_refs()
	{
		tags::tag_instance* SharedTables = reinterpret_cast<tags::tag_instance*>(tags::get_tag_data() + 0x20 + 0xC * *(DWORD*)(tags::get_tag_data() + 4));

		for (int i = 10000; i < tags::get_tag_count(); i++)
			memcpy(&tags::get_tag_instances()[i], &SharedTables[i], sizeof(tags::tag_instance));
	}
	void Generate_sync_list(int type, DWORD index)
	{
		switch ((blam_tag::tag_group_type)type)
		{
		case blam_tag::tag_group_type::biped:
		case blam_tag::tag_group_type::vehicle:
		case blam_tag::tag_group_type::weapon:
		case blam_tag::tag_group_type::garbage:
		case blam_tag::tag_group_type::projectile:
		case blam_tag::tag_group_type::crate:
		case blam_tag::tag_group_type::damageeffect:
		case blam_tag::tag_group_type::device:
		case blam_tag::tag_group_type::scenery:
		case blam_tag::tag_group_type::devicelightfixture:
		case blam_tag::tag_group_type::soundscenery:
		case blam_tag::tag_group_type::creature:
		case blam_tag::tag_group_type::devicemachine:
		case blam_tag::tag_group_type::equipment:
			sync_list.push_back(index);
			break;

		}
	}

	void Add_tags_to_simulation_table()
	{
	}

	datum ResolveNewDatum(int oldDatum)
	{
		for (auto& ref : injected_tag_refs)
		{
			if (ref.old_datum == oldDatum)
				return datum(ref.new_datum);
		}
		return DATUM_NONE;
	}

#pragma region query_parser

	query_parser::query_parser(std::vector<std::string>& vec_query)
	{
		DWORD_list.try_emplace("eax", 0);///lol
		int eax = 0;
		for (size_t i = 0; i < vec_query.size(); i++)
		{
			///only add small code sections here to prevent cluttering
			std::string t = vec_query[i];
			if (t.find("DWORD") != std::string::npos)
			{
				//well we got some variable definitons
				std::string var_name = t.substr(t.find("DWORD") + 6);
				//add, add to list code here
				DWORD_list.try_emplace(var_name, eax);
			}

			else if (t.find("tag_loadEx") != std::string::npos)
			{
				//similar to tag_load,but adds overwriting functionality
				///tag_load(datum_index,overwrite_index,map_name);
				DWORD index = try_parse_int(t.substr(t.find('(') + 1, t.find(',') - (t.find('(') + 1)));
				DWORD overwrite_index = try_parse_int(t.substr(t.find(',') + 1, t.rfind(',') - (t.find(',') + 1)));
				std::string map_name = t.substr(t.rfind(',') + 1, t.rfind(')') - t.rfind(',') - 1);

				if (!index && !overwrite_index)
					return;///before causing any harm

				eax = overwrite_index;
				Load_tag(index, true, map_name);
				Push_Back(overwrite_index);
				DWORD_list["eax"] = eax;
			}
			else if (t.find("tag_load") != std::string::npos)
			{
				//load tag directly from map
				///recursive loading and looks for map in default map folder
				///tag_load(datum_index,map_name)
				DWORD index = try_parse_int(t.substr(t.find('(') + 1, t.find(',') - (t.find('(') + 1)));
				std::string map_name = t.substr(t.find(',') + 1, t.find(')') - (t.find(',') + 1));

				if (!index)
					return;///safely return

				eax = new_datum_index;///store for future use
				Load_tag(index, true, map_name);
				Push_Back();
				DWORD_list["eax"] = eax;
			}
			else if (t.find("module_load") != std::string::npos)
			{
				//gotta load some tag
				std::string s2 = t.substr(t.find('"') + 1, t.rfind('"') - t.find('"') - 1);
				eax = Load_tag_module(tag_loader::cus_tag_dir + "\\" + s2);
				DWORD_list["eax"] = eax;
			}
			else if (t.find("_mov") != std::string::npos)
			{
				//primitive mov function _mov(dest,src)
				std::string dest = t.substr(t.find('(') + 1, t.find(',') - t.find('(') - 1);
				std::string src = t.substr(t.find(',') + 1, t.find(')') - t.find(',') - 1);
				//add call to move function here
				_mov_parser(dest, src);
			}
			else if (t.find("replace_tag") != std::string::npos)
			{
				std::string dest = t.substr(t.find('(') + 1, t.find(',') - t.find('(') - 1);
				std::string src = t.substr(t.find(',') + 1, t.find(')') - t.find(',') - 1);

				replace_tag(dest, src);
			}
			/*
			--no need,will auto add to sync tables
			else if (t.find("sync_tag") != string::npos)
			{
				///incomplete
			}
			*/
		}

	}
	query_parser::query_parser(std::string file_loc)
	{
		//DWORD_list.try_emplace("eax", 0);

		//std::vector<std::string> vec_query;

		//std::ifstream fin;
		//fin.open(file_loc.c_str(), std::ios::in);

		//if (!fin.is_open())
		//{
		//	logs.push_back("Couldnt load query file: " + file_loc);
		//	return;
		//}

		//char query[64];
		//while (!fin.eof())
		//{
		//	fin.getline(query, 64);
		//	std::vector<std::string> temp = clean_string(query);

		//	if (temp.size())
		//		vec_query.insert(vec_query.end(), temp.begin(), temp.end());
		//}
		//fin.close();
		//query_parser::query_parser(vec_query);
	}
	std::vector<std::string> query_parser::clean_string(std::string txt)
	{
		std::vector<std::string> ret;
		std::string temp = "";
		//remove comment sections
		for (size_t i = 0; i < txt.length(); i++)
		{
			if (txt[i] == '/' && txt[i + 1] == '/')
				break;
			temp += txt[i];
		}
		//check for 0 length
		if (!temp.length())
			return ret;

		//codes for handling basic assignment(=)
		///seperate instructions
		int end_pos = temp.length();
		for (int i = temp.length() - 1; i >= 0; i--)
		{
			if (txt[i] == '=')
			{
				ret.push_back(temp.substr(i + 1, end_pos - (i + 1)));
				end_pos = i;
			}
		}
		ret.push_back(temp.substr(0, end_pos));
		//-----Put some trimming operations here

		///apply some fixups for assignment operations
		for (size_t i = 0; i < ret.size(); i++)
		{
			if (!keyword_check(ret[i]))
				if (i == 0)
					ret[i] = "_mov(eax," + ret[i] + ")";
				else ret[i] = "_mov(" + ret[i] + ",eax)";
		}
		return ret;
	}
	int query_parser::try_parse_int(std::string arg)
	{
		//well one could pass funky values at times,i dont want the game crash over here
		try
		{
			//check for hex value
			if (arg.find("0x") == 0)
				return  std::stoul(arg.c_str(), nullptr, 16);
			else //decimal
				return  std::stoul(arg.c_str(), nullptr, 10);
		}
		catch (const std::invalid_argument& ia)
		{
			logs.push_back("Invalid integer : " + arg + ",Plz recheck UR PATHETIC SCRIPT");
		}

		return 0x0;
	}
	std::string query_parser::_getlogs()
	{
		std::string ret = "";
		for (size_t i = 0; i < logs.size(); i++)
			ret += '\n' + logs[i];
		logs.clear();
		return ret;
	}
	void query_parser::_mov_parser(std::string dest, std::string src)
	{
		int val = 0x0;

		if (DWORD_list.find(src) != DWORD_list.end())
			val = DWORD_list[src];
		else val = try_parse_int(src);

		if (DWORD_list.find(dest) != DWORD_list.end())
			DWORD_list[dest] = val;
		else logs.push_back("Undeclared variable : " + dest);
	}
	void query_parser::replace_tag(std::string dest, std::string src)
	{
		int a = 0;
		int b = 0;

		if (DWORD_list.find(dest) != DWORD_list.end())
			a = DWORD_list[dest];
		else a = try_parse_int(dest);

		if (DWORD_list.find(src) != DWORD_list.end())
			b = DWORD_list[src];
		else b = try_parse_int(src);

		auto tag_instance = tags::get_tag_instances();

		//Only replace tags if they do exist
		//Game uses similar method to check if the tag actually exists in the table 
		if (DATUM_ABSOLUTE_INDEX(tag_instance[a & 0xFFFF].datum_index) == (a & 0xFFFF))
		{
			tag_instance[a & 0xFFFF].data_offset = tag_instance[b & 0xFFFF].data_offset;
			tag_instance[a & 0xFFFF].type = tag_instance[b & 0xFFFF].type;
		}


		//Only replace tags if they do exist
		//Game uses similar method to check if the tag actually exists in the table 
		//if (Runtime::Globals::GlobalTagInstances[a & 0xFFFF]->tag_index.Index == (a & 0xFFFF))
			//Runtime::Globals::GlobalTagInstances[a & 0xFFFF]->offset = Runtime::Globals::GlobalTagInstances[b & 0xFFFF]->offset;
	}
	int query_parser::keyword_check(std::string t)
	{
		if (t.find("DWORD") != std::string::npos)
			return 1;
		else if (t.find("module_load") != std::string::npos)
			return 1;
		else if (t.find("_mov") != std::string::npos)
			return 1;
		else if (t.find("replace_tag") != std::string::npos)
			return 1;
		//else if (t.find("sync_tag") != std::string::npos)
			//return 1;
		else if (t.find("tag_loadEx") != std::string::npos)
			return 1;
		else if (t.find("tag_load") != std::string::npos)
			return 1;
		return 0;
	}

#pragma endregion

}
//Used to allocate somemore space for tagtables and tags
unsigned int __cdecl AllocateMemory(int old_size, char arg_4)
{
	typedef unsigned int(_cdecl* Allocate_memory)(int size, char arg_4);
	Allocate_memory pAllocate_memory;
	pAllocate_memory = Memory::GetAddress<Allocate_memory>(0x37E69);

	//i need to allocate more space
	int new_size = old_size + _MAX_ADDITIONAL_TAG_SIZE_;
	tag_loader::def_meta_size = old_size + 0x20;//spacing

	return pAllocate_memory(new_size, arg_4);
}
//function patching to load custom tags
bool _cdecl LoadTagsandMapBases(int a)
{
	// basic load_Tag call
	typedef bool(_cdecl* LoadTagsandSetMapBases)(int a);
	LoadTagsandSetMapBases pLoadTagsandSetMapBases;
	pLoadTagsandSetMapBases = Memory::GetAddress<LoadTagsandSetMapBases>(0x31348);
	bool result = pLoadTagsandSetMapBases(a);

	//Clear the table
	for (auto i = _INJECTED_TAG_START_; i < tag_loader::new_datum_index; i++)
	{
		tag_loader::new_Tables[i] = tags::tag_instance{ blam_tag::none(), DATUM_NONE, 0, 0 };
	}
	tag_loader::que_meta_list.clear();
	tag_loader::key_list.clear();
	// reset starting_datum index
	tag_loader::ext_meta_size = 0;
	tag_loader::new_datum_index = _INJECTED_TAG_START_;

	// adding all shared references
	//tag_loader::Add_all_shared_refs();

	// extending tag_tables and loading tag for all mutiplayer maps and mainmenu map
	if (!tags::get_cache_header()->is_single_player_shared())
	{
		DWORD* TagTableStart = Memory::GetAddress<DWORD*>(0x47CD50);
		memset((BYTE*)tag_loader::new_Tables, 0, 0x3BA40);
		///---------------TABLE EXTENSION  STUFF
		memcpy((BYTE*)tag_loader::new_Tables, (BYTE*)*TagTableStart, 0x3BA40);
		*TagTableStart = (DWORD)tag_loader::new_Tables;
	}


	/*<-------------------------------------------------------------------------------------------------------------------------------------------------------------->
	<-----------------------------------------------------ADD CALLS TO SUBROUTINES BELOW --------------------------------------------------------------------------->
	<--------------------------------------------------------------------------------------------------------------------------------------------------------------->*/


	///tag_injector testing
	//Just for testing purpose,dont cluter here	
	///Actual injection process after map load
	if (!tags::get_cache_header()->is_single_player_shared())
	{
		//actual tag_loading
		///parse query file
		//tag_loader::Parse_query_file(tag_loader::mods_dir + "\\tags\\load_tags.txt");

		//updating SimulationBlock
		tag_loader::Add_tags_to_simulation_table();

		///load via codes
		//tag_loader::Load_tag(0xE19B001D, true, "christmas_hat_map");
		//tag_loader::Load_tag(0xE1BF0024, true, "christmas_hat_map");
		////tag_loader::Dump_Que_meta();
		//tag_loader::Push_Back();


		//Todo :: Make Use of TraceFunctions to Log each step
		//addDebugText(tag_loader::Pop_messages().c_str());
	}

	return result;
}
void _Patch_calls()
{
	//Todo :: Update Offsets for Dedi
	PatchCall(Memory::GetAddress(0x313B2), AllocateMemory);//allocating more space for meta loading
	PatchCall(Memory::GetAddress(0x3166B), LoadTagsandMapBases);//default maps meta loading
	PatchCall(Memory::GetAddress(0x315ED), LoadTagsandMapBases);//custom maps meta loading,i know i am taking risks	

	//client side desync fix
	///(noping out jump instructions)	
	NopFill(Memory::GetAddress(0x316CE), 2);
	NopFill(Memory::GetAddress(0x316DC), 2);
}
void Initialise_tag_loader()
{
	tag_loader::new_Tables = new tags::tag_instance[_MAX_TAG_TABLE_SIZE_];

	std::string game_dir(GetExeDirectoryNarrow());
	std::string def_maps_loc = game_dir + "\\maps";


	tag_loader::mods_dir = game_dir + "\\mods";
	tag_loader::Set_directories(def_maps_loc, "", tag_loader::mods_dir + "\\tags", tag_loader::mods_dir + "\\plugins");//no custom support yet
	_Patch_calls();
}