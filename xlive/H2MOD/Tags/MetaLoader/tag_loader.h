#pragma once


#include "meta_struct.h"
#include "cseries/cseries_strings.h"
#include "H2MOD/Tags/TagInterface.h"

constexpr int _INJECTED_TAG_START_ = 0x3BA4;
constexpr int _MAX_ADDITIONAL_TAG_SIZE_ = 0x1400000; //20 MB
constexpr int _MAX_TAG_TABLE_SIZE_ = 65535;

using meta_struct::meta;
using meta_struct::plugins_field;
using meta_struct::loaded_tag_datum_mapping;

//The TAG LOADER
namespace tag_loader
{
	//returns reference to plugin of specified type
	std::shared_ptr<plugins_field> Get_plugin(std::string type);
	//returns whether the map is a shared map or not
	bool Check_shared(std::ifstream* fin);
	//Checks if the map file exists
	bool Map_exists(std::string map);
	// returns a path from the first directory a matching map name is found
	void find_map_path(const char* map_name, c_static_string260* out_string);
	//Find tag datum based off of tag name
	datum get_tag_datum_by_name(std::string tag_name, e_tag_group type, std::string map);
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
	//function to load RAW_DATA of the concerned tag from meta_list
	//Carefull the tag should be loaded in the meta_tables and meta,this function just fixes its RAW_DATA
	void load_raw_table_data(datum datum_index, c_static_string260* map_loc);
	//Fixes the reference of the tags to their global objects(vftables)
	void setup_injected_tag_havok_vtables(datum datum_index);
	//Fix shader templates
	void initialize_injected_shader_templates();
	//Get new datum from old datum
	datum resolve_cache_index_to_injected(int oldDatum);
}
void Initialise_tag_loader();
