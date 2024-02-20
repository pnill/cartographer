///
//Maphandler stuff ported to C++ by Himanshu01
//uses tinyxml2 for parsing xml plugins
//version -1.1:improvement in extended meta loading logic
///
#pragma once
#include "stdafx.h"
#include "tag_files/tag_groups.h"
#include "tinyxml/tinyxml2.h"

namespace meta_struct
{
	/// <summary>
	/// A struct to contain some info on various SID in the map file
	///	Usually used while generating StringID reference table
	/// </summary>
	struct StringID_info
	{
		int StringID;
		std::string STRING;
	};
	/// <summary>
	/// contains information that is required to update the StringIDs to match the newer map
	/// </summary>
	struct StringIDRef
	{
		int old_SID;
		int new_SID;
	};
	/// <summary>
	/// just a struct to contain data about the meta that are being compiled
	/// </summary>
	struct injectRefs
	{
		int old_datum;
		int new_datum;
	};
	/// <summary>
	/// A class representing the structure halo2 plugins
	/// </summary>
	class plugins_field
	{
		std::string name;
		int offset;
		int entry_size;

		std::map<int, std::string> Tag_refs;//<offset,name>
		std::map<int, std::string> Data_refs;//<offset,name>
		std::map<int, std::string> stringID;//<offset,name>
		std::map<int, std::string> WCTag_refs;//<offset,name>(withClass tagRefs,somewhat different from the normal tags)

		std::list<std::shared_ptr<plugins_field>> reflexive;

	public:
		//constructor
		plugins_field(std::string name, int off, int entry_size);

		int Get_offset();
		int Get_entry_size();
		std::string Get_name();

		void Add_tag_ref(int off, std::string name);
		void Add_data_ref(int off, std::string name);
		void Add_BLOCK(std::shared_ptr<plugins_field> field);
		void Add_stringid_ref(int off, std::string name);
		void Add_WCtag_ref(int off, std::string name);
		std::list<int> Get_tag_ref_list();
		std::list<int> Get_data_ref_list();
		std::list<int> Get_stringID_ref_list();
		std::list<int> Get_WCtag_ref_list();
		std::list<std::shared_ptr<plugins_field>> Get_reflexive_list();
	};
	/// <summary>
	/// a class containing the data of the concerned meta
	/// </summary>
	class meta
	{
		tag_group type;//the type of the meta
		int datum_index;//datum index of the meta data

		int mem_off;// the memory address to which the tag is designed to be loaded at
		int map_off;//offset of the map where the meta is located
		int size;//size of the meta

		std::string map_loc;//location of the concerned map
		std::ifstream* map_stream;//input stream object required for extended meta style metas

		int entry_size;//used for extended_meta reflexive field
		__int8 count;//''''''''''''''''''''''''''''

		char* data;
		bool NO_DELETE;

		std::shared_ptr<plugins_field> plugin;

		std::list<int> ref_reflexive;// a list containing the offset to the location where the reflexive fields are refered in the meta
		std::list<int> ref_tags;//a list containing the offsets to the locations where other tags (datum_indexes) are refered in the meta
		std::list<int> ref_data;//a list containing the offsets to the locations where data_refs are refered in the meta,data refs have similar behaviour as reflexive but it points to a data stuff in the meta
		std::list<int> ref_stringID;//a list containing the offsets to the locations where stringId is refered
		std::list<int> ref_WCtags;//a list containing the offsets of tagRefs with withClass Attribute,they are a bit different as they only contain the datum index of the refered tag

		//i decided to use the memory address of the extended_meta as an identity
		std::map<int, int> ref_extended;//<offset,mem_off>,a dictionary containing the offsets to the locations where the extended meta stuff is referred.
		//here i implement extended_meta in the form of meta
		std::map<int, std::shared_ptr<meta>> list_extended;//<mem_off,meta obj>,a dictionary containing the extended meta by their memory address to prevent redundancy.

		//function to list various dependencies
		//meta_off is specifically used in extended meta cases
		void List_deps(int off, std::shared_ptr<plugins_field> fields);

	public:
		/// used to read meta data from a meta file along with ability to modify mem_off
		///i kept file_loc,datum_index,type for debugging purposes
		meta(char* meta, int size, int mem_off, std::shared_ptr<plugins_field> plugin, std::ifstream* map_stream = nullptr, int map_off = -1, __int8 count = 1, int datum_index = -1, std::string loc = "", tag_group type = { (e_tag_group)NONE});
		//constructor supporting in Game modification of meta loaded into memory
		meta(char* meta, int size, int mem_off, std::shared_ptr<plugins_field> plugin, int count, int datum_index = -1);
		~meta();

		void Rebase_meta(int new_base);//function that changes the mem_offset for which the tag is compiled 
		std::string Update_datum_indexes(std::list<injectRefs>);
		std::string Update_StringID(std::list<StringIDRef> SID_list);
		void null_StringID();

		int Get_Total_size();
		int Get_mem_addr();
		tag_group Get_type();
		std::string Get_map_loc();
		std::list<int> Get_all_tag_refs();
		char* Generate_meta_file();
	};
	//generates a structure from plugin
	std::shared_ptr<plugins_field> Get_Tag_stucture_from_plugin(std::string file_loc);
	std::shared_ptr<plugins_field> Get_meta_BLOCK(tinyxml2::XMLElement* element);

	//some usefull functions	
	std::string Get_file(std::string file_loc);
	std::string Get_file_type(std::string file);
	std::string Get_file_directory(std::string file_loc);
	std::string to_hex_string(int);
}
