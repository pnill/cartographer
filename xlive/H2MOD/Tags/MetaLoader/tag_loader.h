#pragma once

#include "stdafx.h"
#include "meta_struct.h"
#include "cache_loader.h"
#include "H2MOD/Tags/TagInterface.h"

constexpr int _INJECTED_TAG_START_ = 0x3BA4;
constexpr int _MAX_ADDITIONAL_TAG_SIZE_ = 0x1400000; //20 MB
constexpr int _MAX_TAG_TABLE_SIZE_ = 65535;

using meta_struct::meta;
using meta_struct::plugins_field;
using meta_struct::injectRefs;

//The TAG LOADER
namespace tag_loader
{
	//returns reference to plugin of specified type
	std::shared_ptr<plugins_field> Get_plugin(std::string type);
	//returns whether the map is a shared map or not
	bool Check_shared(std::ifstream* fin);
	//Loads a tag from specified map in accordance with the datum index supplied
	void Load_tag(int datum_index, bool recursive, std::string map, bool custom = false);
	//Return the size of the meta that is currently in the que
	unsigned int Que_meta_size();
	//Generates a SID ref table
	int Generate_SID(int table_index, int set, std::string STRING);
	//Returns a list of strings along with their stringIDs
	std::list<meta_struct::StringID_info> Get_SID_list(std::string map_loc);
	//sets various directories required for working of tag stuff
	void Set_directories(std::string default_maps, std::string custom_maps, std::string custom_tags, std::string plugin_loc);
	//Updates datum_indexes and rebases tags before inserting into memory
	//pushes the tag_data in que to the tag_tables and tag_memory in the custom_tags allocated space
	void Push_Back();
	//pushes the tag_data in que to the tag_tables and tag_memory at specified tag_table index
	//BECAREFULL WHILE OVERWRITING TAGS
	void Push_Back(int datum_index);	
	//Dumps meta data in que in the specified tag folder(integrity checking)
	void Dump_Que_meta();
	//return and clears all the error messages incurred
	std::string Pop_messages();
	//return a tag_name list
	std::string Pop_tag_list();
	//function to load RAW_DATA of the concerned tag from meta_list
	//Carefull the tag should be loaded in the meta_tables and meta,this function just fixes its RAW_DATA
	void Load_RAW_refs(datum datum_index, std::string map_loc);
	//same as the previous implementation,but it utilises file handle to load from any file 
	void Load_RAW_refs(datum datum_index, HANDLE file);
	//Fixes the reference of the tags to their global objects(vftables)
	void Fix_global_objects_ref(datum datum_index);
	//Loads a file containing a a bunch of tags concerned with some specific element injecting the first tag onto the target datum
	//modules cand be target onto a specific tag,they have to be loaded into free tag mem pool
	//return the target_index of the first tag in the memory
	int Load_tag_module(std::string loc);
	//loades and execute instructions in the query file
	void Parse_query_file(std::string loc);
	//Adds reference of all the tags present in shared map into the tag_table
	void Add_all_shared_refs();
	//verifies and adds the Datum to  sync_tags list for the module loading
	void Generate_sync_list(int type, DWORD index);
	//Adds the valid tags to GlobalScenario->SimulationDefinitionTable Block and clears the sync list
	void Add_tags_to_simulation_table();
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
		void _mov_parser(std::string dest, std::string src);
		//void replace_tag
		void replace_tag(std::string dest, std::string src);
		//check for keywords and standard functions and returns appropriately
		int keyword_check(std::string);
	public:
		//reference to vector of queries
		query_parser(std::vector<std::string>&);
		//complete file location
		query_parser(std::string file_loc);
		//return all logs
		std::string _getlogs();
	};
}
void Initialise_tag_loader();

