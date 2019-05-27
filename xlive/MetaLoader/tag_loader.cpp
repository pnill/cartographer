#include"..\stdafx.h"
#include"tag_loader.h"
#include "..\Globals.h"
#include "..\Util\filesys.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include"..\Runtime\RuntimeIncludes.h"

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
	map<int, shared_ptr<meta>> meta_list;//<datum_index,meta_junk>contains the the list of tags that are currently loaded in memory
	vector<string> error_list;//contains various messages generated during various processes,shouldnt had named it error list

	unsigned int tag_count = 0x0;//unitialised
	unsigned int def_meta_size = 0x0;//uninitialised
	unsigned int ext_meta_size = 0x0;//uninitialised
	unsigned int new_datum_index = 0x3BA4;//first datum index
	char* new_Tables;//new tables pointer

	string meta_list_target_map;//name of the map for which the meta list has been target(used for StringIDs)
	string que_list_target_map;//name of the map from which the que list has been loaded(used for StringID retargeting)

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
		char map_name[0x24];
		fin->seekg(0x1A4);
		fin->read(map_name, 0x24);

		string temp_str = map_name;

		int ret = temp_str.find("shared", 0);

		if (ret >= 0)
			return true;
		else return false;
	}
	//Loads a tag from specified map in accordance with the datum index supplied
	//and rebases it to 0x0
	void Load_tag(int datum_index, bool recursive, string map, bool custom)
	{
		que_list_target_map = map;
		string map_loc;

		if (!custom)
		{
			if (meta_struct::Get_file_type(map) == "map")
				map_loc = def_maps_dir + '\\' + map;
			else
				map_loc = def_maps_dir + '\\' + map + ".map";
		}
		else
		{
			if (meta_struct::Get_file_type(map) == "map")
				map_loc = cus_maps_dir + '\\' + map;
			else
				map_loc = cus_maps_dir + '\\' + map + ".map";
		}

		ifstream* fin = new ifstream(map_loc.c_str(), ios::binary | ios::in);

		if (fin->is_open())
		{
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
							temp_meta->Rebase_meta(0x0);
							temp_meta->null_StringID();//<<---------------------------------fix this LATER ON

							que_meta_list.emplace(*(load_tag_list.cbegin()), temp_meta);

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
							string temp_error = "Invalid Datum index :0x" + meta_struct::to_hex_string(datum_index);
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
	unsigned int meta_size()
	{
		unsigned int ret = 0;
		for (int i = 0; i < meta_list.size(); i++)
			ret += meta_list[i]->Get_Total_size();

		return ret;
	}
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
		int que_size = que_meta_list.size();
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
		map<int, shared_ptr<meta_struct::meta>>::iterator que_iter = que_meta_list.begin();

		bool replaced = false;
		while (que_iter != que_meta_list.end())
		{
			injectRefs temp;
			temp.old_datum = que_iter->first;

			if (!replaced)
			{
				if (datum_index != new_datum_index)
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

			que_iter++;
		}
		//StringID listing


		//update the que list tags
		que_iter = que_meta_list.begin();
		while (que_iter != que_meta_list.end())
		{
			que_iter->second->Update_datum_indexes(my_inject_refs);
			//<stringID stuff>
			//-------------IDC anymore----------------

			que_iter++;
		}
		//Add them to the tables
		que_iter = que_meta_list.begin();
		list<injectRefs>::iterator my_inject_refs_iter = my_inject_refs.begin();
		while (que_iter != que_meta_list.end())
		{
			int meta_size = que_iter->second->Get_Total_size();

			if (def_meta_size)
			{
				char tables_data[0x10];
				DWORD MapMemBase = *(DWORD*)(h2mod->GetBase() + 0x47CD64);

				que_iter->second->Rebase_meta(mem_off);
				char* meta_data = que_iter->second->Generate_meta_file();

				char type[5];
				memcpy(type, que_iter->second->Get_type().c_str(), 0x5);
				reverse(&type[0], &type[4]);
				memcpy(tables_data, type, 0x4);

				memcpy(tables_data + 0x4, &my_inject_refs_iter->new_datum, 0x4);
				memcpy(tables_data + 0x8, &mem_off, 0x4);
				memcpy(tables_data + 0xC, &meta_size, 0x4);
				int temp_write_off = (DWORD)tag_loader::new_Tables + 0x10 * (my_inject_refs_iter->new_datum & 0xFFFF);
				memcpy((char*)temp_write_off, tables_data, 0x10);//copy to the tables

				memcpy((char*)MapMemBase + mem_off, meta_data, meta_size);//copy to the tag memory

				//fix the global_refs
				Fix_global_objects_ref(my_inject_refs_iter->new_datum);
				//Load RAW
				Load_RAW_refs(my_inject_refs_iter->new_datum, que_iter->second->Get_map_loc());

				delete[] meta_data;
				mem_off += meta_size;
			}
			meta_list.emplace(my_inject_refs_iter->new_datum, que_iter->second);//add it to the meta _list
			ext_meta_size += meta_size;		

			que_iter++;
			my_inject_refs_iter++;
		}
		my_inject_refs.clear();
		que_meta_list.clear();
	}
	//function to reinject the meta in meta_list upon map reload
	void Reinject_meta()
	{
		//reset some stuff
		int mem_off = def_meta_size;
		new_datum_index = 0x3BA4;
		ext_meta_size = 0x0;

		map<int, shared_ptr<meta_struct::meta>>::iterator meta_list_iter = meta_list.begin();
		while (meta_list_iter != meta_list.end())
		{
			if (tag_count >= 0x2710 || meta_list_iter->first >= 0x3BA4)
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
	//clears the tags in que_list
	void Clear_que_list()
	{
		que_meta_list.clear();
	}
	//Dumps meta data in que in the specified tag folder
	void Dump_Que_meta()
	{
		ofstream fout;

		map<int, shared_ptr<meta>>::const_iterator i = que_meta_list.cbegin();

		while (i != que_meta_list.cend())
		{
			string file_loc = cus_tag_dir + '\\' + meta_struct::to_hex_string(i->first);

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
	//function to load RAW_DATA of the concerned tag from meta_list
	//Carefull the tag should be loaded in the meta_tables and meta,this function just fixes its RAW_DATA
	void Load_RAW_refs(int datum_index,string map_loc)
	{
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
		HANDLE new_file_handle = CreateFileA(map_loc.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if (new_file_handle == INVALID_HANDLE_VALUE)
		{
			//i suppose its in scenario format
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
				std::reverse(&type[0], &type[strlen(type)]);
				int mem_off = *(int*)&t_ptr[i + 0x8];
				int size = *(int*)&t_ptr[i + 0xC];

				*(int*)&t_ptr[i + 0x4] = new_datum_index;//update the datum_index in the tag_tables

				t_ref.old_datum = tags_first_d_index++;
				t_ref.new_datum = new_datum_index++;
				
				shared_ptr<plugins_field> t_plugin = Get_plugin(type);
				shared_ptr<meta> t_meta = make_shared<meta>(d_ptr, size, mem_off, t_plugin, 1, t_ref.old_datum);

				my_inject_refs.push_back(t_ref);
				module_tags.push_back(t_meta);

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
		}
		else
		{
			string error = "Couldnt find module :" + loc;
			error_list.push_back(error);
		}
		delete my_loader;

		return ret;
	}
	//speed up and simiplify the injection process
	void Parse_query_file(string loc)
	{
		std::ifstream fin;
		fin.open(loc.c_str(), ios::in);

		if (!fin.is_open())
			return;
		int eax;

		std::string s1;
		char query[64];
		while (!fin.eof())
		{
			fin.getline(query, 64);
			s1 = query;
			if (s1[0] != '#')
			{
				
				if (s1.find("module_load") != string::npos)
				{
					std::string s2 = s1.substr(s1.find('"') + 1, s1.rfind('"') - s1.find('"') - 1);
					eax = Load_tag_module(meta_struct::Get_file_directory(loc) + "\\" + s2);
				}
				else if (s1.find("spawn") != string::npos)
				{

				}
				else if (s1.find("spawn()") != string::npos)
				{
					
				}				
			}
		}
	}
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
	typedef char(_cdecl *LoadTagsandSetMapBases)(int a);
	LoadTagsandSetMapBases pLoadTagsandSetMapBases;
	pLoadTagsandSetMapBases = (LoadTagsandSetMapBases)((char*)h2mod->GetBase() + 0x31348);
	char result = pLoadTagsandSetMapBases(a);

	//only for multiplayer injection
	if (Runtime::Cache::CacheHeader->type == Blam::Cache::cache_header::scnr_type::Multiplayer)
	{
		DWORD *TagTableStart = (DWORD*)(h2mod->GetBase() + 0x47CD50);
		//TABLE EXTENSION  STUFF
		memcpy((BYTE*)tag_loader::new_Tables, (BYTE*)*TagTableStart, 0x3BA40);
		*TagTableStart = (DWORD)tag_loader::new_Tables;

		tag_loader::tag_count = *(DWORD*)(*(DWORD*)(h2mod->GetBase() + 0x47D568) + 0x18);
		tag_loader::Reinject_meta();

		/*
		//grunt
		tag_loader::Load_tag(0xEC020A8C, true, "03a_oldmombasa.map", false);
		tag_loader::Push_Back();
		*/

		//tag_loader::Load_tag_module("C:\\h2v\\ff_arena_wall_bloc.cache");

		tag_loader::Parse_query_file(tag_loader::mods_dir + "\\tags\\load_tags.txt");

		addDebugText(tag_loader::Pop_messages().c_str());

	}
	return result;
}
void _Patch_calls()
{
	PatchCall(h2mod->GetBase() + 0x313B2, (DWORD)AllocateMemory);//allocating more space for meta loading
	PatchCall(h2mod->GetBase() + 0x3166B, (DWORD)LoadTagsandMapBases);//default maps meta loading
	PatchCall(h2mod->GetBase() + 0x315ED, (DWORD)LoadTagsandMapBases);//custom maps meta loading,i know i am taking risks
}
void Initialise_tag_loader()
{
	tag_loader::new_Tables = new char[_MAX_TAG_TABLE_SIZE_];

	string game_dir = GetExeDirectoryNarrow();
	string def_maps_loc = game_dir + "\\maps";

	//addDebugText(game_path.c_str());
	//addDebugText(def_maps_loc.c_str());

	tag_loader::mods_dir = game_dir + "\\mods";
	tag_loader::Set_directories(def_maps_loc, "", tag_loader::mods_dir + "\\tags", tag_loader::mods_dir + "\\plugins");//no custom support yet
	_Patch_calls();
}