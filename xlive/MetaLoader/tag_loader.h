///
//Tag loader made by Himanshu01
//version -1.1 
///
//<--------------Notes---------------------->
///v1.0
//Cache loader is quit completed(its not blam loader)
//The query parser can currently have module loading and spawn_commands(non-functional[lol])
///v1.1
//Removed call to reinjection function as the query parser makes it obselete 
//Changed the check_shared function definition
//Removed the NullString id instruction in Load_tag function,beware it may cause crashes when directly loading from a single player map
//Reworked query parser
//Added replace tag command
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

//The TAG LOADER
//All the StringID related stuff are incomplete
namespace tag_loader
{
	//returns reference to plugin of specified type
	shared_ptr<plugins_field> Get_plugin(string type);
	//returns whether the map is a shared map or not
	bool Check_shared(ifstream* fin);
	//Loads a tag from specified map in accordance with the datum index supplied
	void Load_tag(int datum_index, bool recursive, string map, bool custom = false);
	//Return the size of the meta that is currently in the que
	unsigned int Que_meta_size();
	//Return the size of the meta that has been injected or would be injected upon map load
	//unsigned int meta_size();
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
	//void Reinject_meta();
	//Dumps meta data in que in the specified tag folder(integrity checking)
	void Dump_Que_meta();
	//return and clears all the error messages incurred
	string Pop_messages();
	//return a tag_name list
	string Pop_tag_list();
	//Generates a StringId List combining all the default maps
	//void Dump_StringID_list();
	//Generates a StringId List for a specific map and adds it to the list
	//void Dump_StringID_list(string map_loc);
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
	//Adds reference of all the tags present in shared map into the tag_table
	void Add_all_shared_refs();
	///
	//<-----------------The query parser or more of a script parser------------------------------->
	///
	class query_parser
	{
	private:
		std::unordered_map<std::string, int> DWORD_list;//hash table containing the variables allocated in the script   
		//try parsing to int
		int try_parse_int(std::string);
		//bunch of log text
		std::vector<std::string> logs;
		//remove comments ,simplify assignment and others
		std::vector<std::string> clean_string(std::string);
		//_mov parser
		void _mov_parser(string dest, string src);
		//void replace_tag
		void replace_tag(string dest, string src);
		//check for keywords and standard functions and returns appropriately
		int keyword_check(std::string);
	public:
		//reference to vector of queries
		query_parser(std::vector<std::string>&);
		//complete file location
		query_parser(std::string file_loc);
		//return all logs
		string _getlogs();
	};
}
void Initialise_tag_loader();

