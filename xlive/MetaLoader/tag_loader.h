///
//Tag loader 1.0 madde by Himanshu01
//version -1.0 :Basic loading and dumping
///
//Cache loader is quit completed(its not blam loader)
//The query parser can currently have module loading and spawn_commands
///
#pragma once

#define _MAX_TAG_TABLE_SIZE_ 0xFFFF0
#define _MAX_ADDITIONAL_TAG_SIZE_  0x1400000//20 MB

#include"..\stdafx.h"
#include"meta_struct.h"
#include"cache_loader.h"
#include<fstream>
#include<vector>
#include<algorithm>
#include<list>

using std::vector;
using std::list;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::reverse;
using meta_struct::meta;
using meta_struct::plugins_field;
using meta_struct::injectRefs;

//the TAG LOADER
//All the StringID related stuff are incomplete
namespace tag_loader
{
	//returns reference to plugin of specified type
	shared_ptr<plugins_field> Get_plugin(string type);
	//returns whether the map is a shared map or not
	bool Check_shared(ifstream* fin);
	//Loads a tag from specified map in accordance with the datum index supplied
	//and rebases it to 0x0
	void Load_tag(int datum_index, bool recursive, string map, bool custom = false);
	//Return the size of the meta that is currently in the que
	unsigned int Que_meta_size();
	//Return the size of the meta that has been injected or would be injected upon map load
	unsigned int meta_size();
	//Generates a SID ref table
	int Generate_SID(int table_index, int set, string STRING);
	//Returns a list of strings along with their stringIDs
	list<meta_struct::StringID_info> Get_SID_list(string map_loc);
	//sets various directories required for working of tag stuff
	void Set_directories(string default_maps, string custom_maps, string custom_tags, string plugin_loc);
	//Updates datum_indexes and rebases tags before inserting into memory
	//pushes the tag_data in que to the tag_tables and tag_memory in the custom_tags allocated space
	void Push_Back();
	//pushes the tag_data in que to the tag_tables and tag_memory at specified tag_table index
	//BECAREFULL WHILE OVERWRITING TAGS
	void Push_Back(int datum_index);	
	//reinjects the meta upon map reload or map change
	void Reinject_meta();
	//Dumps meta data in que in the specified tag folder
	void Dump_Que_meta();
	//return and clears all the error messages incurred
	string Pop_messages();
	//Generates a StringId List combining all the default maps
	void Dump_StringID_list();
	//Generates a StringId List for a specific map and adds it to the list
	void Dump_StringID_list(string map_loc);
	//function to load RAW_DATA of the concerned tag from meta_list
	//Carefull the tag should be loaded in the meta_tables and meta,this function just fixes its RAW_DATA
	void Load_RAW_refs(int datum_index, string map_loc);
	//Fixes the reference of the tags to their global objects(vftables)
	void Fix_global_objects_ref(int datum_index);
	//Loads a file containing a a bunch of tags concerned with some specific element injecting the first tag onto the target datum
	//modules cand be target onto a specific tag,they have to be loaded into free tag mem pool
	//return the target_index of the first tag in the memory
	int Load_tag_module(string loc);
	//loades and execute instructions in the query file
	void Parse_query_file(string loc);
}
void Initialise_tag_loader();