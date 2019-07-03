#include"..\stdafx.h"
#include"tag_loader.h"
#include "..\Globals.h"
#include "..\Util\filesys.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include"..\Runtime\RuntimeIncludes.h"
#include "Shlwapi.h"
#include"..\Blam\Cache\Tags\tag_definitons.h"

//contains some game functions that returns HANDLE
namespace global_handle_function
{
	HANDLE __cdecl get_map_Handle_from_scnr(const char *pScenario)
	{
		return	((HANDLE(__cdecl *)(const char*))((char*)h2mod->GetBase() + 0x38607))(pScenario);
	}
}
//certain functions which relate tags to their global object(such as Havok objects)(vftables perhaps)
namespace global_objects_fix
{
	void __cdecl bipd_fix(unsigned __int16 datum_index)
	{
		void(_cdecl*sub_EC23F)(unsigned __int16);
		sub_EC23F = (void(_cdecl*)(unsigned __int16))((char*)h2mod->GetBase() + 0x1389B0);
		sub_EC23F(datum_index);
	}

	void __cdecl crea_fix(unsigned __int16 datum_index)
	{
		int(_cdecl*sub_EC23F)(unsigned __int16);
		sub_EC23F = (int(_cdecl*)(unsigned __int16))((char*)h2mod->GetBase() + 0x138985);
		sub_EC23F(datum_index);
	}
	void __cdecl vehi_fix(unsigned __int16 datum_index)
	{
		int(_cdecl*sub_EC23F)(unsigned __int16);
		sub_EC23F = (int(_cdecl*)(unsigned __int16))((char*)h2mod->GetBase() + 0x13895A);
		sub_EC23F(datum_index);
	}
	void __cdecl coll_fix(unsigned __int16 datum_index)
	{
		int(_cdecl*sub_EC23F)(unsigned __int16);
		sub_EC23F = (int(_cdecl*)(unsigned __int16))((char*)h2mod->GetBase() + 0x7BE5C);
		sub_EC23F(datum_index);
	}
	void __cdecl phmo_fix(unsigned __int16 datum_index)
	{
		int(_cdecl*sub_EC23F)(unsigned __int16, int);
		sub_EC23F = (int(_cdecl*)(unsigned __int16, int))((char*)h2mod->GetBase() + 0x7B844);
		sub_EC23F(datum_index, 0);
	}
}
//actual tag_loader namespace
namespace tag_loader
{
	string def_maps_dir;
	string cus_maps_dir;
	string cus_tag_dir;
	string plugins_dir;
	string mods_dir;

	static map<string, shared_ptr<plugins_field>> plugins_list;//contains list of various plugin structures
	map<int, shared_ptr<meta>> que_meta_list;//<datum_index,meta_junk>contains the list of tags that are currently in que to loaded in memory
	vector<int> key_list;//another var just to keep the keys along with the correct order
	//map<int, shared_ptr<meta>> meta_list;//<datum_index,meta_junk>contains the the list of tags that are currently loaded in memory(never gonna use it anyway)
	vector<string> error_list;//contains various messages generated during various processes,shouldnt had named it error list
	vector<string> tag_list;//contains a list of tag_indices along with their names(currently implemented only for module loading)

	unsigned int tag_count = 0x0;//unitialised
	unsigned int def_meta_size = 0x0;//uninitialised
	unsigned int ext_meta_size = 0x0;//uninitialised
	unsigned int new_datum_index = _INJECTED_TAG_START_;//first datum index
	char* new_Tables;//new tables pointer

	string meta_list_target_map;//name of the map for which the meta list has been target(used for StringIDs)
	string que_list_target_map;//name of the map from which the que list has been loaded(used for StringID retargeting)
	vector<DWORD> sync_list;//container to store tag indices of sync-able Tags

	shared_ptr<plugins_field> Get_plugin(string type)
	{
		map<string, shared_ptr<plugins_field>>::iterator i = plugins_list.begin();

		//we first look into the already loaded plugin list
		while (i != plugins_list.end())
		{
			if (i->first == type)
				return i->second;
			i++;
		}
		//it doesnt contain it therfore we need to load the plugin
		string plugin_loc = plugins_dir + '\\' + type + ".xml";
		shared_ptr<plugins_field> temp_plugin = meta_struct::Get_Tag_stucture_from_plugin(plugin_loc);

		if (temp_plugin)		
			plugins_list.emplace(type, temp_plugin);
		else
		{
			string error = "Couldnt load plugin " + type + ".xml";
			error_list.push_back(error);
		}

		return temp_plugin;
	}
	//returns whether the map is a shared map or not
	bool Check_shared(ifstream* fin)
	{
		char* map_header= new char[0x800];
		fin->seekg(0x0);
		fin->read(map_header, 0x800);
		
		Blam::Cache::cache_header* ptr = (Blam::Cache::cache_header*)map_header;

		if (ptr->type == Blam::Cache::cache_header::scnr_type::MultiplayerShared|| ptr->type == Blam::Cache::cache_header::scnr_type::SinglePlayerShared)
		{
			delete[] map_header;
			return true;
		}
		delete[] map_header;
		return false;
	}
	//Loads a tag from specified map in accordance with the datum index supplied
	///custom flag is no more needed
	void Load_tag(int datum_index, bool recursive, string map, bool custom)
	{
		ifstream* fin;

		que_list_target_map = map;
		string map_loc;

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

		fin = new ifstream(map_loc.c_str(), ios::binary | ios::in);

		if (fin->is_open())
		{
			string temp_string = "Loading tag : " + meta_struct::to_hex_string(datum_index) + " from " + map;
			error_list.push_back(temp_string);

			//some meta reading prologue
			int table_off, table_size;

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


			char type[5];
			int Tdatum_index, mem_off, size;
			type[4] = '\0';


			//create a tag_list to load
			list<int> load_tag_list;
			load_tag_list.push_back(datum_index);

			//----------------LOOPING STUFF
			while (load_tag_list.size())
			{
				if (*load_tag_list.cbegin() != -1 && *load_tag_list.cbegin() != 0)
				{
					//method to read tag type
					fin->seekg(table_start + (0xFFFF & *(load_tag_list.cbegin())) * 0x10);

					fin->read((char*)&type, 4);
					reverse(&type[0], &type[4]);
					fin->read((char*)&Tdatum_index, 4);
					fin->read((char*)&mem_off, 4);
					fin->read((char*)&size, 4);

					if (*(load_tag_list.cbegin()) == Tdatum_index)
					{
						shared_ptr<plugins_field> temp_plugin = Get_plugin(type);

						//precaution for plugin load errors
						if (!temp_plugin)
							break;

						//we first check the integrity of the datum_index
						if (Tdatum_index == *(load_tag_list.cbegin()) && mem_off&&size && (que_meta_list.find(Tdatum_index) == que_meta_list.cend()))
						{
							//read the meta data from the map            
							char* data = new char[size];

							int map_off;
							if (!Check_shared(fin))
								map_off = scnr_off + (mem_off - scnr_memaddr);
							else
								map_off = scnr_off + (mem_off - 0x3c000);
							fin->seekg(map_off);
							//0x3c000 is a hardcoded value in blam engine

							fin->read(data, size);

							//create a meta object
							shared_ptr<meta> temp_meta = make_shared<meta>(data, size, mem_off, temp_plugin, fin, map_off, 1, *(load_tag_list.cbegin()), map_loc, type);
							//temp_meta->Rebase_meta(0x0);
							//found unnecessary

							que_meta_list.emplace(*(load_tag_list.cbegin()), temp_meta);
							key_list.push_back(*(load_tag_list.cbegin()));

							if (recursive)
							{
								list<int> temp_tag_ref = temp_meta->Get_all_tag_refs();

								//to remove redundancies
								list<int>::iterator ref_iter = temp_tag_ref.begin();
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
							string temp_error = "Invalid Datum index :0x" + meta_struct::to_hex_string(*(load_tag_list.cbegin()));
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
			string temp_error;
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
		map<int, shared_ptr<meta_struct::meta>>::iterator i = que_meta_list.begin();

		while (i != que_meta_list.end())
		{
			ret += i->second->Get_Total_size();
			i++;
		}

		return ret;
	}
	//Return the size of the meta that has been injected or would be injected upon map load
	/*
	unsigned int meta_size()
	{
		unsigned int ret = 0;
		for (int i = 0; i < meta_list.size(); i++)
			ret += meta_list[i]->Get_Total_size();

		return ret;
	}
	*/

	/// <summary>
	/// 
	/// </summary>
	/// <param name="table_index"></param>
	/// <param name="STRING"></param>
	/// <returns></returns>
	int Generate_SID(int table_index, int set, string STRING)
	{
		int l = (STRING.length() & 0xFF) << 24;
		int s = (set & 0xFF) << 16;
		int t = table_index & 0xFFFF;

		return (l | s | table_index);
	}
	///
	//Returns a list of strings along with their stringIDs
	///
	list<meta_struct::StringID_info> Get_SID_list(string map_loc)
	{
		list<meta_struct::StringID_info> ret;

		ifstream fin;
		fin.open(map_loc.c_str(), ios::in | ios::binary);

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

			string STRING = "";

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
	void Set_directories(string default_maps, string custom_maps, string custom_tags, string plugin_loc)
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
			string error = "Coudnt inject,Max meta size reached";
			error_list.push_back(error);
		}
	}
	//pushes the tag_data in que to the tag_tables and tag_memory at specified tag_table index
	//usually i call this to overwrite tag_table of some preloaded tag(for replacing purpose)
	void Push_Back(int datum_index)
	{
		int mem_off = def_meta_size + ext_meta_size;

		//build up inject refs
		list<injectRefs> my_inject_refs;

		bool replaced = false;
		for (int i = 0; i < key_list.size(); i++)
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
		string temp = "Pushing back tag : " + meta_struct::to_hex_string(my_inject_refs.begin()->old_datum) + " to : " + meta_struct::to_hex_string(my_inject_refs.begin()->new_datum);
		error_list.push_back(temp);

		//StringID listing---IDC


		//update the que list tags
		for (int i=0;i<key_list.size();i++)
		{
			que_meta_list[key_list[i]]->Update_datum_indexes(my_inject_refs);
			//<stringID stuff>
			//-------------IDC anymore----------------
		}
		//Add them to the tables
		list<injectRefs>::iterator my_inject_refs_iter = my_inject_refs.begin();
		while (my_inject_refs_iter != my_inject_refs.end())
		{		

			if (def_meta_size)
			{
				int meta_size = que_meta_list[my_inject_refs_iter->old_datum]->Get_Total_size();
				char tables_data[0x10];
				DWORD MapMemBase = *(DWORD*)(h2mod->GetBase() + 0x47CD64);

				que_meta_list[my_inject_refs_iter->old_datum]->Rebase_meta(mem_off);
				char* meta_data = que_meta_list[my_inject_refs_iter->old_datum]->Generate_meta_file();

				char type[5];
				memcpy(type, que_meta_list[my_inject_refs_iter->old_datum]->Get_type().c_str(), 0x5);
				reverse(&type[0], &type[4]);
				memcpy(tables_data, type, 0x4);

				memcpy(tables_data + 0x4, &my_inject_refs_iter->new_datum, 0x4);
				memcpy(tables_data + 0x8, &mem_off, 0x4);
				memcpy(tables_data + 0xC, &meta_size, 0x4);
				int temp_write_off = (DWORD)tag_loader::new_Tables + 0x10 * (my_inject_refs_iter->new_datum & 0xFFFF);
				memcpy((char*)temp_write_off, tables_data, 0x10);//copy to the tables

				memcpy((char*)MapMemBase + mem_off, meta_data, meta_size);//copy to the tag memory

				//Load RAW
				Load_RAW_refs(my_inject_refs_iter->new_datum, que_meta_list[my_inject_refs_iter->old_datum]->Get_map_loc());
				//fix the global_refs
				Fix_global_objects_ref(my_inject_refs_iter->new_datum);

				delete[] meta_data;
				mem_off += meta_size;
				ext_meta_size += meta_size;
			}
			else break;
			//meta_list.emplace(my_inject_refs_iter->new_datum, que_iter->second);//add it to the meta _list				
			my_inject_refs_iter++;
		}
		my_inject_refs.clear();
		que_meta_list.clear();
	}
	/*
	//function to reinject the meta in meta_list upon map reload
	void Reinject_meta()
	{
		//reset some stuff
		int mem_off = def_meta_size;
		new_datum_index = _INJECTED_TAG_START_;
		ext_meta_size = 0x0;

		map<int, shared_ptr<meta_struct::meta>>::iterator meta_list_iter = meta_list.begin();
		while (meta_list_iter != meta_list.end())
		{
			if (tag_count >= 0x2710 || meta_list_iter->first >= _INJECTED_TAG_START_)
			{
				int meta_size = meta_list_iter->second->Get_Total_size();

				char tables_data[0x10];
				DWORD MapMemBase = *(DWORD*)(h2mod->GetBase() + 0x47CD64);
				meta_list_iter->second->Rebase_meta(mem_off);
				char* meta_data = meta_list_iter->second->Generate_meta_file();

				char type[5];
				memcpy(type, meta_list_iter->second->Get_type().c_str(), 0x5);
				reverse(&type[0], &type[4]);
				memcpy(tables_data, type, 0x4);

				memcpy(tables_data + 0x4, &meta_list_iter->first, 0x4);
				memcpy(tables_data + 0x8, &mem_off, 0x4);
				memcpy(tables_data + 0xC, &meta_size, 0x4);
				char* temp_write_off = tag_loader::new_Tables + 0x10 * (meta_list_iter->first & 0xFFFF);
				memcpy((char*)temp_write_off, tables_data, 0x10);//copy to the tables

				memcpy((char*)MapMemBase + mem_off, meta_data, meta_size);//copy to the tag memory

				//fix the global_refs
				Fix_global_objects_ref(meta_list_iter->first);
				//Load RAW
				Load_RAW_refs(meta_list_iter->first, meta_list_iter->second->Get_map_loc());

				delete[] meta_data;
				mem_off += meta_size;
				ext_meta_size += meta_size;
				new_datum_index = meta_list_iter->first;
			}
			meta_list_iter++;
		}
	}
	//clears the tags in meta_list
	void Clear_meta_list()
	{
		meta_list.clear();
		ext_meta_size = 0x0;
	}
	*/
	//clears the tags in que_list
	void Clear_que_list()
	{
		que_meta_list.clear();
	}
	//Rebases to 0x0 and dumps meta data in que in the specified tag folder
	void Dump_Que_meta()
	{
		ofstream fout;

		map<int, shared_ptr<meta>>::const_iterator i = que_meta_list.cbegin();

		while (i != que_meta_list.cend())
		{
			string file_loc = cus_tag_dir + "\\que\\" + meta_struct::to_hex_string(i->first);

			i->second->Rebase_meta(0x0);
			int size = i->second->Get_Total_size();
			char* data = i->second->Generate_meta_file();
			string type = i->second->Get_type();

			file_loc += '.' + type;
			fout.open(file_loc, ios::out | ios::binary);

			fout.write(data, size);


			delete[] data;
			fout.close();
			i++;
		}

	}
	//return and clears all the error messages incurred
	string Pop_messages()
	{
		string ret;
		while (error_list.size())
		{
			ret = error_list[error_list.size() - 1] + '\n' + ret;
			error_list.pop_back();
		}
		return ret;
	}
	//return and clears all the loaded tag list
	string Pop_tag_list()
	{
		string ret;
		while (tag_list.size())
		{
			ret = tag_list[tag_list.size() - 1] + '\n' + ret;
			tag_list.pop_back();
		}
		return ret;
	}
	/*
	*isnt complete yet
	//Generates a StringId List combining all the default maps
	void Dump_StringID_list()
	{
		vector<string> test;
		read_directory(def_maps_dir, test);//default maps

		for (int i = 0; i < test.size(); i++)
		{
			if (meta_struct::Get_file_type(test[i]) == "map")
				Dump_StringID_list(def_maps_dir + '\\' + test[i]);
		}
		test.clear();

		read_directory(cus_maps_dir, test);//custom maps
		for (int i = 0; i < test.size(); i++)
		{
			if (meta_struct::Get_file_type(test[i]) == "map")
				Dump_StringID_list(cus_maps_dir + '\\' + test[i]);
		}

		test.clear();

	}	
	//Generates a StringId List for a specific map and adds it to the list
	void Dump_StringID_list(string map_loc)
	{
		list<meta_struct::StringID_info> SID_list = Get_SID_list(map_loc);
		cache_loader my_cache_loader("C:\\h2v\\SID_list.bin");
		cache_BLOCK* strings_block = my_cache_loader.get_BLOCK("strings");

	}
	*/
	//function to try and return a handle to the map (map_name or scenario_name(same as the actual map_name) supported)
	//Checks inside mods//maps folder first then maps folder and finally inside custom maps folder
	HANDLE try_find_map(string map)
	{
		string map_loc = map;
		//checking for full path length
		if (!PathFileExistsA(map_loc.c_str()))
		{
			if (map.find('\\') == string::npos)
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
				string map_name = map_loc.substr(map_loc.rfind('\\') + 1);
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
	void Load_RAW_refs(int datum_index, string map_loc)
	{
		DWORD* PMapRawtableoffset = (DWORD*)(h2mod->GetBase() + 0x4AE8B0);
		DWORD* PRawTableSize = (DWORD*)(h2mod->GetBase() + 0x4AE8B4);

		//a little  precaution to circumvent unexpected behaviour
		DWORD oldRtable_offset = *PMapRawtableoffset;
		DWORD oldRtable_size = *PRawTableSize;

		*PMapRawtableoffset = 0x0;
		*PRawTableSize = 0x0;

		DWORD SharedmapBase = *(DWORD*)(h2mod->GetBase() + 0x47CD64);
		DWORD ETCOFFSET = *(DWORD*)(h2mod->GetBase() + 0x482290);
		HANDLE old_file_handle = *(HANDLE*)(h2mod->GetBase() + 0x4AE8A8);

		//char* ripped_map = (char*)(SharedmapBase + tag_scenario_off);

		int type = *(int*)(new_Tables + (datum_index & 0xFFFF) * 0x10);
		int Tdatum_index = *(int*)(0x10 * (datum_index & 0xFFFF) + new_Tables + 0x4);//just to verify
		int tag_mem_addr = SharedmapBase + *(int*)(0x10 * (datum_index & 0xFFFF) + new_Tables + 0x8);
		int tag_size = *(int*)(0x10 * (datum_index & 0xFFFF) + new_Tables + 0xC);

		//fail safe
		if (Tdatum_index != datum_index)
		{
			string error = "Tag :" + datum_index;
			error += " not loaded into tag tables and tag memory";;
			throw new exception(error.c_str());
		}

		//supposing full length
		HANDLE new_file_handle = try_find_map(map_loc);
		if (new_file_handle == INVALID_HANDLE_VALUE)
		{
			//i suppose its in scenario fomat and placed in default and custom directories
			new_file_handle = global_handle_function::get_map_Handle_from_scnr(map_loc.c_str());
		}
		*(HANDLE*)(h2mod->GetBase() + 0x4AE8A8) = new_file_handle;

		switch (type)
		{
		case 0x6D6F6465://mode

			if (*(int*)(tag_mem_addr + 0x24) > 0)
			{
				int v15 = 0;

				int off = 0;
				do
				{
					int sections_off = *(int*)(tag_mem_addr + 0x28);
					int sections_base = 0;
					if (sections_off != 0xFFFFFFFF)
						sections_base = ETCOFFSET + sections_off;
					((void(__cdecl *)(int, unsigned int))((char*)h2mod->GetBase() + 0x2652BC))(sections_base + off + 0x38, 3u);
					++v15;
					off += 0x5C;
				} while (v15 < *(int*)(tag_mem_addr + 0x24));
			}
			break;

		case 0x6269746D://bitm
		{

			int old_list_field = *(DWORD*)(h2mod->GetBase() + 0xA49270 + 0x1FC);

			for (int i = 0; i < *(int*)(tag_mem_addr + 0x44); i++)
			{

				int bitmaps_field_off = *(int*)(tag_mem_addr + 0x48);

				int bitmaps_field_base = 0;
				if (bitmaps_field_off != 0xFFFFFFFF)
					bitmaps_field_base = bitmaps_field_off + ETCOFFSET;

				int bitmaps_field = bitmaps_field_base + 0x74 * i;

				*(DWORD*)(h2mod->GetBase() + 0xA49270 + 0x1FC) = bitmaps_field;

				int temp = 0;
				((int(__cdecl *)(int, char, int, void*))((char*)h2mod->GetBase() + 0x265986))(bitmaps_field, 2, 0, &temp);

				((int(__cdecl *)(int, char, int, void*))((char*)h2mod->GetBase() + 0x265986))(bitmaps_field, 0, 0, &temp);

			}
			*(DWORD*)(h2mod->GetBase() + 0xA49270 + 0x1FC) = old_list_field;
			break;
		}
		default:
			break;
		}
		*(HANDLE*)(h2mod->GetBase() + 0x4AE8A8) = old_file_handle;
		CloseHandle(new_file_handle);

		*PMapRawtableoffset = oldRtable_offset;
		*PRawTableSize = oldRtable_size;
	}
	//Fixes the reference of the tags to their global objects(vftables)
	void Fix_global_objects_ref(int datum_index)
	{
		int type = *(int*)(new_Tables + (datum_index & 0xFFFF) * 0x10);
		int Tdatum_index = *(int*)(new_Tables + (datum_index & 0xFFFF) * 0x10 + 0x4);

		if (Tdatum_index != datum_index)
		{
			string error = "Tag :" + datum_index;
			error += " not loaded into tag tables and tag memory";;
			throw new exception(error.c_str());
		}

		switch (type)
		{
		case 0x63726561://crea
			global_objects_fix::crea_fix(datum_index);
			break;
		case 0x62697064://bipd
			global_objects_fix::bipd_fix(datum_index);
			break;
		case 0x636F6C6C://coll
			global_objects_fix::coll_fix(datum_index);
			break;
		case 0x70686D6F://phmo
			global_objects_fix::phmo_fix(datum_index);
			break;
		case 0x76656869://vehi
			global_objects_fix::vehi_fix(datum_index);
			break;
		default:
			break;
		}
	}
	//Basically created to easily transfer working tags from system to system
	int Load_tag_module(string loc)
	{
		int ret = -1;

		if (!new_Tables)
			return 0;

		cache_loader* my_loader = new cache_loader(loc);

		if (my_loader->get_last_error()!=UNABLE_TO_LOCATE_FILE)
		{
			cache_BLOCK* module_tag_table = my_loader->get_BLOCK("tag_table");
			cache_BLOCK* module_tag_data = my_loader->get_BLOCK("tag_data");
			cache_BLOCK* module_tag_maps = my_loader->get_BLOCK("tag_maps");
			cache_BLOCK* module_tag_names = my_loader->get_BLOCK("tag_names");

			if (ext_meta_size + module_tag_data->size > _MAX_ADDITIONAL_TAG_SIZE_)
			{
				string error = "Couldnt load module,MAX_ADDITIONAL_TAG_SIZE_ reached";
				error_list.push_back(error);
				delete my_loader;

				return ret;
			}
			ret = new_datum_index;
			string error = "Loading module :" + loc+" to datum_index "+meta_struct::to_hex_string(new_datum_index);
			error_list.push_back(error);

			DWORD MapMemBase = *(DWORD*)(h2mod->GetBase() + 0x47CD64);
			int mem_off = def_meta_size + ext_meta_size;

			char* t_ptr = new_Tables + new_datum_index * 0x10;
			char* d_ptr = (char*)MapMemBase + mem_off;
			char* m_ptr = module_tag_maps->data;
			char* n_ptr = module_tag_names->data;
			if (n_ptr == nullptr)
				throw new std::exception("tag_names block not found");
			//copying tables
			memcpy(t_ptr, module_tag_table->data, module_tag_table->size);
			memcpy(d_ptr, module_tag_data->data, module_tag_data->size);

			list<injectRefs> my_inject_refs;
			vector<shared_ptr<meta>> module_tags;

			int tags_first_d_index = *(int*)&t_ptr[4];

			for (int i = 0; i < module_tag_table->size; i += 0x10)
			{				
				injectRefs t_ref;

				char type[5];
				type[4] = '\0';
				*(int*)type = *(int*)&t_ptr[i];

				int type_rev = *(int*)type;

				std::reverse(&type[0], &type[strlen(type)]);
				int mem_off = *(int*)&t_ptr[i + 0x8];
				int size = *(int*)&t_ptr[i + 0xC];

				*(int*)&t_ptr[i + 0x4] = new_datum_index;//update the datum_index in the tag_tables

				t_ref.old_datum = tags_first_d_index++;
				t_ref.new_datum = new_datum_index++;//assign and increment it for next tag
				
				shared_ptr<plugins_field> t_plugin = Get_plugin(type);
				shared_ptr<meta> t_meta = make_shared<meta>(d_ptr, size, mem_off, t_plugin, 1, t_ref.old_datum);

				my_inject_refs.push_back(t_ref);
				module_tags.push_back(t_meta);

				std::string t_name = n_ptr;
				tag_list.push_back(t_name.substr(t_name.rfind('\\') + 1) + ",0x" + meta_struct::to_hex_string(t_ref.new_datum));
				tag_loader::Generate_sync_list(&type_rev, t_ref.new_datum);

				n_ptr += t_name.size() + 1;
				d_ptr += size;
			}
			for (int i = 0; i < module_tags.size(); i++)
			{
				int new_mem_off = def_meta_size + ext_meta_size;

				*(int*)&t_ptr[i * 0x10 + 0x8] = new_mem_off;

				module_tags[i]->Update_datum_indexes(my_inject_refs);
				module_tags[i]->Rebase_meta(new_mem_off);

				Load_RAW_refs(*(int*)&t_ptr[i * 0x10 + 0x4], m_ptr);
				Fix_global_objects_ref(*(int*)&t_ptr[i * 0x10 + 0x4]);

				//as the tags are from the same map
				//m_ptr += strlen(m_ptr) + 1;
				ext_meta_size += module_tags[i]->Get_Total_size();
			}
			module_tags.clear();
		}
		else
		{
			string error = "Couldnt find module :" + loc;
			error_list.push_back(error);
		}
		delete my_loader;

		return ret;
	}
	//Simiplify the injection process
	void Parse_query_file(string loc)
	{
		query_parser* my_parser = new query_parser(loc);
		addDebugText(my_parser->_getlogs().c_str());
		delete my_parser;
	}
	void Add_all_shared_refs()
	{
		DWORD SharedMemBase = *(DWORD*)(h2mod->GetBase() + 0x47CD54);
		DWORD SharedTables = SharedMemBase + 0x20 + 0xC * *(DWORD*)(SharedMemBase + 4);
		DWORD TagTableStart = *(DWORD*)(h2mod->GetBase() + 0x47CD50);

		for (int i = 0x2710; i < tag_loader::tag_count; i++)
			memcpy((void*)(TagTableStart + i * 0x10), (void*)(SharedTables + i * 0x10), 0x10);
	}
	void Generate_sync_list(void* type, DWORD index)
	{
		//type += '\0';
		int t = *(int*)type;
		//std::reverse((char*)&t, (char*)&t + 4);
		
		switch((TagGroupTypes)t)
		{
		case TagGroupTypes::biped:
		case TagGroupTypes::vehicle:
		case TagGroupTypes::weapon:
		case TagGroupTypes::garbage:
		case TagGroupTypes::projectile:
		case TagGroupTypes::crate:
		case TagGroupTypes::damageeffect:
		case TagGroupTypes::device:
		case TagGroupTypes::scenery:
		case TagGroupTypes::devicelightfixture:
		case TagGroupTypes::soundscenery:
		case TagGroupTypes::creature:
		case TagGroupTypes::devicemachine:
		case TagGroupTypes::equipment:
			sync_list.push_back(index);
			break;
			
		}		
			
	}
	void Add_tags_to_simulation_table()
	{
		if (sync_list.size() == 0)
			return;
		else
		{
			auto tag_offset_header = *(DWORD*)(h2mod->GetAddress(0x47D568));
			DatumIndex scnr_index = *(DWORD*)(tag_offset_header + 0xC);//Scneario DatumIndex

			auto GlobalSCNR = (Blam::Cache::Tags::scnr*)Runtime::Globals::GlobalTagInterface.GetTagInterface(scnr_index, (int)TagGroupTypes::scenario);

			for (int i = 0 ; i < sync_list.size(); i++)
			{
				string t;

				t += "Adding Tag To Simulation Block : 0x";
				t += meta_struct::to_hex_string(GlobalSCNR->SimulationDefinitionTable.GetElementCount()) + ",0x";
				t += meta_struct::to_hex_string(sync_list[i]);

				addDebugText(t.c_str());

				struct Blam::Cache::Tags::scnr::SimulationDefinitionTable block;
				block.Tag.TagIndex = sync_list[i];

				GlobalSCNR->SimulationDefinitionTable.PushBack(&block);
			}		
			//clearn out the sync list for this module
			sync_list.clear();
		}
	}

#pragma region query_parser

	query_parser::query_parser(std::vector<std::string>& vec_query)
	{
		DWORD_list.try_emplace("eax", 0);///lol
		int eax = 0;
		for (int i = 0; i < vec_query.size(); i++)
		{
			///only add small code sections here to prevent cluttering
			string t = vec_query[i];
			if (t.find("DWORD") != string::npos)
			{
				//well we got some variable definitons
				std::string var_name = t.substr(t.find("DWORD") + 6);
				//add, add to list code here
				DWORD_list.try_emplace(var_name, eax);
			}
			
			else if (t.find("tag_loadEx") != string::npos)
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
			else if (t.find("tag_load") != string::npos)
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
			else if (t.find("module_load") != string::npos)
			{
				//gotta load some tag
				std::string s2 = t.substr(t.find('"') + 1, t.rfind('"') - t.find('"') - 1);
				eax = Load_tag_module(tag_loader::cus_tag_dir + "\\" + s2);
				DWORD_list["eax"] = eax;
			}
			else if (t.find("_mov") != string::npos)
			{
				//primitive mov function _mov(dest,src)
				std::string dest = t.substr(t.find('(') + 1, t.find(',') - t.find('(') - 1);
				std::string src = t.substr(t.find(',') + 1, t.find(')') - t.find(',') - 1);
				//add call to move function here
				_mov_parser(dest, src);
			}
			else if (t.find("replace_tag") != string::npos)
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
		DWORD_list.try_emplace("eax", 0);

		std::vector<std::string> vec_query;

		std::ifstream fin;
		fin.open(file_loc.c_str(), ios::in);

		if (!fin.is_open())
		{
			logs.push_back("Couldnt load query file:" + file_loc);
			return;
		}

		char query[64];
		while (!fin.eof())
		{
			fin.getline(query, 64);
			std::vector<std::string> temp = clean_string(query);

			if (temp.size())
				vec_query.insert(vec_query.end(), temp.begin(), temp.end());
		}
		fin.close();
		query_parser::query_parser(vec_query);
	}
	std::vector<std::string> query_parser::clean_string(std::string txt)
	{
		std::vector<std::string> ret;
		string temp="";
		//remove comment sections
		for (int i = 0; i < txt.length(); i++)
		{
			if (txt[i] == '/'&&txt[i + 1] == '/')
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
		for (int i = 0; i < ret.size(); i++)
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
	string query_parser::_getlogs()
	{
		string ret = "";
		for (int i = 0; i < logs.size(); i++)
			ret += '\n' + logs[i];
		logs.clear();
		return ret;
	}
	void query_parser::_mov_parser(string dest, string src)
	{
		int val = 0x0;

		if (DWORD_list.find(src) != DWORD_list.end())
			val = DWORD_list[src];
		else val = try_parse_int(src);
		
		if (DWORD_list.find(dest) != DWORD_list.end())
			DWORD_list[dest] = val;
		else logs.push_back("Undeclared variable : " + dest);
	}
	void query_parser::replace_tag(string dest, string src)
	{
		int a = 0;
		int b = 0;

		if (DWORD_list.find(dest) != DWORD_list.end())
			a = DWORD_list[dest];
		else a = try_parse_int(dest);

		if (DWORD_list.find(src) != DWORD_list.end())
			b = DWORD_list[src];
		else b = try_parse_int(src);
		
		auto tag_instance=(Blam::EngineDefinitions::Tag::global_tag_instance*)*Runtime::Globals::GlobalTagInstances;

		//Only replace tags if they do exist
		//Game uses similar method to check if the tag actually exists in the table 
		if (tag_instance[a & 0xFFFF].tag_index.Index == (a & 0xFFFF))
		{
			tag_instance[a & 0xFFFF].offset = tag_instance[b & 0xFFFF].offset;
			tag_instance[a & 0xFFFF].type = tag_instance[b & 0xFFFF].type;
		}
		

		//Only replace tags if they do exist
		//Game uses similar method to check if the tag actually exists in the table 
		//if (Runtime::Globals::GlobalTagInstances[a & 0xFFFF]->tag_index.Index == (a & 0xFFFF))
			//Runtime::Globals::GlobalTagInstances[a & 0xFFFF]->offset = Runtime::Globals::GlobalTagInstances[b & 0xFFFF]->offset;
	}
	int query_parser::keyword_check(std::string t)
	{
		if (t.find("DWORD") != string::npos)
			return 1;
		else if (t.find("module_load") != string::npos)
			return 1;
		else if (t.find("_mov") != string::npos)
			return 1;
		else if (t.find("replace_tag") != string::npos)
			return 1;
		//else if (t.find("sync_tag") != string::npos)
			//return 1;
		else if (t.find("tag_loadEx") != string::npos)
			return 1;
		else if (t.find("tag_load") != string::npos)
			return 1;
		return 0;
	}

#pragma endregion

}
//Used to allocate somemore space for tagtables and tags
unsigned int __cdecl AllocateMemory(int old_size, char arg_4)
{
	typedef unsigned int(_cdecl *Allocate_memory)(int size, char arg_4);
	Allocate_memory pAllocate_memory;
	pAllocate_memory = (Allocate_memory)((char*)h2mod->GetBase() + 0x37E69);

	//i need to allocate more space
	int new_size = old_size + _MAX_ADDITIONAL_TAG_SIZE_;
	tag_loader::def_meta_size = old_size + 0x20;//spacing

	return pAllocate_memory(new_size, arg_4);
}
//function patching to load custom tags
char _cdecl LoadTagsandMapBases(int a)
{
	//basic load_Tag call
	typedef char(_cdecl *LoadTagsandSetMapBases)(int a);
	LoadTagsandSetMapBases pLoadTagsandSetMapBases;
	pLoadTagsandSetMapBases = (LoadTagsandSetMapBases)((char*)h2mod->GetBase() + 0x31348);
	char result = pLoadTagsandSetMapBases(a);
	
	//reset starting_datum index
	tag_loader::new_datum_index = _INJECTED_TAG_START_;
	tag_loader::ext_meta_size = 0x0;

	//just storing tag_couunt
	tag_loader::tag_count = *(DWORD*)(*(DWORD*)(h2mod->GetBase() + 0x47D568) + 0x18);

	//adding all shared references
	tag_loader::Add_all_shared_refs();

	//extending tag_tables and loading tag for all mutiplayer maps and mainmenu map
	if (Runtime::Cache::CacheHeader->type != Blam::Cache::cache_header::scnr_type::SinglePlayer)
	{
		DWORD *TagTableStart = (DWORD*)(h2mod->GetBase() + 0x47CD50);
		///---------------TABLE EXTENSION  STUFF
		memcpy((BYTE*)tag_loader::new_Tables, (BYTE*)*TagTableStart, 0x3BA40);
		*TagTableStart = (DWORD)tag_loader::new_Tables;
	}


/*<-------------------------------------------------------------------------------------------------------------------------------------------------------------->
<-----------------------------------------------------ADD CALLS TO SUBROUTINES BELOW --------------------------------------------------------------------------->
<--------------------------------------------------------------------------------------------------------------------------------------------------------------->*/


	/*Test Codes

	auto tag_offset_header = *(DWORD*)(h2mod->GetAddress(0x47D568));
	DatumIndex scnr_index = *(DWORD*)(tag_offset_header + 0xC);//Scneario DatumIndex

	auto GlobalSCNR = (Blam::Cache::Tags::scnr*)Runtime::Globals::GlobalTagInterface.GetTagInterface(scnr_index, (int)TagGroupTypes::scenario);

	addDebugText("Sync List Dump0");
	for (int i = 0; i < GlobalSCNR->SimulationDefinitionTable.GetElementCount(); i++)
	{
		string t = "Datum : 0x" + meta_struct::to_hex_string(GlobalSCNR->SimulationDefinitionTable[i]->Tag.TagIndex.ToInt()) + " has index : " + meta_struct::to_hex_string(i);
		addDebugText(t.c_str());
	}
	*/


	///tag_injector testing
	//Just for testing purpose,dont cluter here	
	///Actual injection process after map load
	if (Runtime::Cache::CacheHeader->type != Blam::Cache::cache_header::scnr_type::SinglePlayer)
	{
		//actual tag_loading
		///parse query file
		tag_loader::Parse_query_file(tag_loader::mods_dir + "\\tags\\load_tags.txt");

		//updating SimulationBlock
		tag_loader::Add_tags_to_simulation_table();

		///load via codes
		//tag_loader::Load_tag(0xE886001D, true, "dreamer");
		//tag_loader::Dump_Que_meta();

		//Todo :: Make Use of TraceFunctions to Log each step
		addDebugText(tag_loader::Pop_messages().c_str());
	}

	///
	//global_tag_interface testing code
	//Please dont add every code over here and cluter it,create newer subroutine
	///
	
	if (Runtime::Cache::CacheHeader->type != Blam::Cache::cache_header::scnr_type::MainMenu)
	{
		DatumIndex temp(0xE1940018);
		auto test = (Blam::Cache::Tags::itmc*)Runtime::Globals::GlobalTagInterface.GetTagInterface(temp, (int)TagGroupTypes::itemcollection);
		
		if (false)
		{
			/*
			//resolve ambiguity
			struct Blam::Cache::Tags::itmc::ItemPermutations itemperm;

			itemperm.Weight = 100.0f;

			itemperm.Item.TagGroup = TagGroupTypes::vehicle;
			itemperm.Item.TagIndex = 0xE7A42D3F;
			test->ItemPermutations.PushBack(&itemperm);

			itemperm.Item.TagGroup = TagGroupTypes::biped;
			itemperm.Item.TagIndex = 0xE35028EB;
			test->ItemPermutations.PushBack(&itemperm);
			*/

			//test->ItemPermutations.RemoveAt(0);
			test->ItemPermutations[0]->Item.TagGroup = TagGroupTypes::weapon;
			test->ItemPermutations[0]->Item.TagIndex = 0x3BA4;
		}
	}	

	return result;
}
void _Patch_calls()
{
	PatchCall(h2mod->GetBase() + 0x313B2, (DWORD)AllocateMemory);//allocating more space for meta loading
	PatchCall(h2mod->GetBase() + 0x3166B, (DWORD)LoadTagsandMapBases);//default maps meta loading
	PatchCall(h2mod->GetBase() + 0x315ED, (DWORD)LoadTagsandMapBases);//custom maps meta loading,i know i am taking risks	

	//client side desync fix
	///(noping out jump instructions)	
	NopFill<2>(h2mod->GetBase() + 0x316CE);
	NopFill<2>(h2mod->GetBase() + 0x316DC);
}
void Initialise_tag_loader()
{
	tag_loader::new_Tables = new char[_MAX_TAG_TABLE_SIZE_];

	string game_dir = GetExeDirectoryNarrow();
	string def_maps_loc = game_dir + "\\maps";


	tag_loader::mods_dir = game_dir + "\\mods";
	tag_loader::Set_directories(def_maps_loc, "", tag_loader::mods_dir + "\\tags", tag_loader::mods_dir + "\\plugins");//no custom support yet
	_Patch_calls();
}