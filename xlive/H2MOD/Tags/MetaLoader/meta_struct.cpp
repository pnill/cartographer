#include "meta_struct.h"
#include "Blam/Cache/DataTypes/BlamTag.h"

namespace meta_struct
{
	///
	//<--------------------------------plugins_field members definition------------------------------------------------>
	///
	plugins_field::plugins_field(std::string name, int off, int entry_size)
	{
		//initialse some stuff
		this->name = name;
		this->offset = off;
		this->entry_size = entry_size;
	}
	int plugins_field::Get_offset()
	{
		return this->offset;
	}
	int plugins_field::Get_entry_size()
	{
		return this->entry_size;
	}
	std::string plugins_field::Get_name()
	{
		return this->name;
	}
	void plugins_field::Add_tag_ref(int off, std::string name)
	{
		Tag_refs.try_emplace(off, name);
	}
	void plugins_field::Add_data_ref(int off, std::string name)
	{
		Data_refs.try_emplace(off, name);
	}
	void plugins_field::Add_BLOCK(std::shared_ptr<plugins_field> field)
	{
		reflexive.push_back(field);
	}
	void plugins_field::Add_stringid_ref(int off, std::string name)
	{
		stringID.try_emplace(off, name);
	}
	void plugins_field::Add_WCtag_ref(int off, std::string name)
	{
		WCTag_refs.try_emplace(off, name);
	}
	std::list<int> plugins_field::Get_tag_ref_list()
	{
		std::list<int> ret;
		for (auto& i : Tag_refs)
		{
			ret.push_back(i.first);
		}
		return ret;
	}
	std::list<int> plugins_field::Get_data_ref_list()
	{
		std::list<int> ret;

		for (auto& i : Data_refs)
		{
			ret.push_back(i.first);
		}
		return ret;
	}
	std::list<int> plugins_field::Get_stringID_ref_list()
	{
		std::list<int> ret;

		for (auto& i : stringID)
		{
			ret.push_back(i.first);
		}
		return ret;
	}
	std::list<int> plugins_field::Get_WCtag_ref_list()
	{
		std::list<int> ret;

		for (auto& i : WCTag_refs)
		{
			ret.push_back(i.first);
		}
		return ret;
	}
	std::list<std::shared_ptr<plugins_field>> plugins_field::Get_reflexive_list()
	{
		return reflexive;
	}
	///
	//<-------------------------Tag_Structure retrieving functions from xml plugin------------------------------------------>
	///
	std::shared_ptr<plugins_field> Get_Tag_stucture_from_plugin(std::string file_loc)
	{
		tinyxml2::XMLDocument xmlDoc;
		tinyxml2::XMLError error = xmlDoc.LoadFile(file_loc.c_str());

		if (error == tinyxml2::XML_ERROR_FILE_NOT_FOUND)
		{
			std::string exception_text = "Couldnt find file :" + file_loc;
			throw new std::exception(exception_text.c_str());
		}
		tinyxml2::XMLElement* root_element = xmlDoc.RootElement();

		int base_size = std::stoul(root_element->Attribute("baseSize"), nullptr, 16);

		std::shared_ptr<plugins_field> ret = std::make_shared<plugins_field>(Get_file(file_loc), 0x0, base_size);

		tinyxml2::XMLElement* child_element = root_element->FirstChildElement();
		while (child_element)
		{
			std::string element_name = child_element->Name();

			if (element_name == "reflexive")
			{
				ret->Add_BLOCK(Get_meta_BLOCK(child_element));
			}
			else if ((element_name == "tagRef") || (element_name == "tagref"))
			{
				int off = std::stoul(child_element->Attribute("offset"), nullptr, 16);

				if (!child_element->BoolAttribute("withClass"))
					ret->Add_tag_ref(off, child_element->Attribute("name"));
				else
					ret->Add_WCtag_ref(off, child_element->Attribute("name"));
			}
			else if ((element_name == "stringId") || (element_name == "stringid"))
			{
				int off = std::stoul(child_element->Attribute("offset"), nullptr, 16);
				ret->Add_stringid_ref(off, child_element->Attribute("name"));
			}
			else if (element_name == "dataref")
			{
				int off = std::stoul(child_element->Attribute("offset"), nullptr, 16);
				ret->Add_data_ref(off, child_element->Attribute("name"));
			}
			child_element=child_element->NextSiblingElement();
		}
		return ret;
	}
	std::shared_ptr<plugins_field> Get_meta_BLOCK(tinyxml2::XMLElement* element)
	{
		int offset = std::stoul(element->Attribute("offset"), nullptr, 16);
		int entry_size = std::stoul(element->Attribute("entrySize"), nullptr, 16);

		std::shared_ptr<plugins_field> ret = std::make_shared<plugins_field>(element->Attribute("name"), offset, entry_size);

		//iterate through all the child elements
		tinyxml2::XMLElement* child_element = element->FirstChildElement();
		while (child_element)
		{
			std::string element_name = child_element->Name();

			if (element_name == "reflexive")
			{
				ret->Add_BLOCK(Get_meta_BLOCK(child_element));
			}
			else if ((element_name == "tagRef") || (element_name == "tagref"))
			{
				std::string name = child_element->Attribute("name");

				int off = std::stoul(child_element->Attribute("offset"), nullptr, 16);

				if (!child_element->BoolAttribute("withClass") && !child_element->Attribute("withClass"))
					ret->Add_tag_ref(off, name);
				else
					ret->Add_WCtag_ref(off, name);
			}
			else if ((element_name == "stringId") || (element_name == "stringid"))
			{
				std::string name = child_element->Attribute("name");

				int off = std::stoul(child_element->Attribute("offset"), nullptr, 16);
				ret->Add_stringid_ref(off, name);
			}
			else if (element_name == "dataref")
			{
				std::string name = child_element->Attribute("name");

				int off = std::stoul(child_element->Attribute("offset"), nullptr, 16);
				ret->Add_data_ref(off, name);
			}
			child_element=child_element->NextSiblingElement();
		}
		return ret;
	}
	///
	//<------------------------------meta structure---------------------------------------------------->
	///
	//constructor for in memory loading and rebasing
	//houses both meta and extended meta types
	meta::meta(char* meta, int size, int mem_off, std::shared_ptr<plugins_field> plugin, std::ifstream* map_stream, int map_off, __int8 count,  int datum_index , std::string loc, blam_tag type)
	{
		this->data = meta;
		this->size = size;
		this->mem_off = mem_off;
		this->plugin = plugin;
		this->map_stream = map_stream;
		this->map_off = map_off;		
		this->datum_index = datum_index;
		this->map_loc = loc;
		this->type = type;

		//this meta isnt loaded into in game tag_memory block
		NO_DELETE = false;

		if (!size)
			return;

		if (count > 0)
			this->entry_size = size / count;
		else
		{
			std::string exp = "count cannot be zero";
			throw new std::exception(exp.c_str());
		}

		this->count = count;

		if (plugin != nullptr)
			for (int i = 0; i < count; i++)
				List_deps(entry_size*i, plugin);

	}
	//a constructor to utilise tag loaded into memory and to modify them
	//currently for rebasing and reassigning datum indices
	meta::meta(char* meta, int size, int mem_off, std::shared_ptr<plugins_field> plugin, int count, int datum_index)
	{
		this->data = meta;
		this->size = size;
		this->mem_off = mem_off;
		this->plugin = plugin;
		this->map_stream = nullptr;
		this->map_off = -1;
		this->datum_index = datum_index;
		this->map_loc = -1;
		this->type = blam_tag::none();

		NO_DELETE = true;

		if (!size)
			return;

		if (count > 0)
			this->entry_size = size / count;
		else
		{
			std::string exp = "count cannot be zero";
			throw new std::exception(exp.c_str());
		}

		this->count = count;

		if (plugin != nullptr)
			for (int i = 0; i < count; i++)
				List_deps(entry_size*i, plugin);

	}
	//function that lists various dependencies of the meta
	void meta::List_deps(int off, std::shared_ptr<plugins_field> fields)
	{
		std::list<int> temp = fields->Get_tag_ref_list();

		//first we look for tag_refs and add them
		for (int& i : temp)
		{
			int Toff = off + i;//it contains type
							   //we add this off to the list if it doesnt contain the off already
			if (find(ref_tags.begin(), ref_tags.end(), Toff) == ref_tags.end())
			{
				ref_tags.push_back(Toff);
			}
		}
		//then we look for data_refs and add them
		//but they have some issue,some dataref refers data outside of the tag
		//so we have to be a bit carefull
		temp = fields->Get_data_ref_list();
		for (int& i : temp)
		{
			int Toff = off + i;

			int length = *(int*)(data + Toff);
			int field_memaddr = *(int*)(data + Toff + 0x4);

			int field_off = field_memaddr - mem_off;//its the offset of the field from the starting of the meta data

			if (length != 0)
			{
				//now we check whether its inside meta or a case of extended meta
				if ((field_memaddr >= mem_off) && (field_off < size))
				{
					//inside meta
					//we add this off to the list if it doesnt contain the off already
					if (find(ref_data.begin(), ref_data.end(), Toff) == ref_data.end())
					{
						ref_data.push_back(Toff);
					}
				}
				else
				{
					//Here i am using the same method used in the case extended reflexive fields.
					//extended meta(IN A SUCCESSFULL RUN ,EXTENDED META ONLY APPEARS ONLY WHEN WE READ FROM A MAP)
					if (map_stream != nullptr)
					{
						//we add this off to the list if it doesnt contain the off already
						if (ref_extended.find(Toff) == ref_extended.end())
						{
							ref_extended.emplace(Toff, field_memaddr);
							//now we create and add extended_meta to the list if it isnt already there
							if (list_extended.find(field_memaddr) == list_extended.end())
							{
								//for extended meta we have to read the map file and supply it to the meta object
								char* ext_data = new char[length];

								map_stream->seekg(map_off + (field_memaddr - mem_off));
								map_stream->read(ext_data, length);

								std::shared_ptr<meta> temp_extend = std::make_shared<meta>(ext_data, length, field_memaddr, nullptr);
								list_extended.emplace(field_memaddr, temp_extend);
							}
							//we dont need to look into them as extended meta does it for us
						}
					}
					else
					{
						//the program will only reach here when u try to use an extended meta on meta file.
						//any meta which i extract from a map file have all issues of extended_meta fixed.
						std::string exp = "Meta file 0x" + to_hex_string(datum_index) + "." + type.as_string() + " is broken.\nEither debug the extraction proceedure or fix the meta file";
						throw new std::exception(exp.c_str());
					}
				}

			}

		}
		//then we look for stringId refs and add them
		temp = fields->Get_stringID_ref_list();
		for (int& i : temp)
		{
			int Toff = off + i;
			//we add this off to the list if it doesnt contain the off already
			if (find(ref_stringID.begin(), ref_stringID.end(), Toff) == ref_stringID.end())
			{
				ref_stringID.push_back(Toff);
			}
		}
		//now we look into reflexive fields and extended meta and add them accordingly
		std::list<std::shared_ptr<plugins_field>> Ptemp = fields->Get_reflexive_list();
		for (auto& i_Pfield : Ptemp)
		{
			int Toff = off + i_Pfield->Get_offset();//field table off contains count

			int count = *(int*)(data + Toff);
			int field_memaddr = *(int*)(data + Toff + 0x4);
			int entry_size = i_Pfield->Get_entry_size();//entry_size

			int field_off = field_memaddr - mem_off;//its the offset of the field from the starting of the meta data
			if (count > 0)
			{
				//now we check whether its inside meta or a case of extended meta
				if ((field_memaddr >= mem_off) && (field_off < size))
				{
					//inside meta
					//we add this off which contains reflexive table to the list if it doesnt contain the off already
					if (find(ref_reflexive.begin(), ref_reflexive.end(), Toff) == ref_reflexive.end())
					{
						ref_reflexive.push_back(Toff);
						//after adding it to the list we look into them,recursively
						for (int j = 0; j < count; j++)
						{
							List_deps(field_off + j * entry_size, i_Pfield);
						}
					}

				}
				else
				{
					//extended meta(IN SUCCESSFULL RUN ,EXTENDED META ONLY APPEARS ONLY WHEN WE READ FROM A MAP)

					//but first we check whether we are reading meta from a map,or an exracted file,its rather easy
					if (map_stream != nullptr)
					{
						//we add this off to the list if it doesnt contain the off already
						if (ref_extended.find(Toff) == ref_extended.end())
						{
							ref_extended.emplace(Toff, field_memaddr);
							//now we create and add extended_meta to the list if it isnt already there
							if (list_extended.find(field_memaddr) == list_extended.end())
							{
								//for extended meta we have to read the map file and supply it to the meta object
								int length = entry_size * count;

								char* ext_data = new char[length];
								int extended_meta_map_off = map_off + (field_memaddr - mem_off);

								map_stream->seekg(extended_meta_map_off);
								map_stream->read(ext_data, length);

								std::shared_ptr<meta> temp_extend = std::make_shared<meta>(ext_data, length, field_memaddr, i_Pfield, map_stream, extended_meta_map_off, count);
								list_extended.emplace(field_memaddr, temp_extend);
							}
							//we dont need to look into them as extended meta does it for us
						}
					}
					else
					{
						//the program will only reach here when u try to use an extended meta on meta file.
						//any meta which i extract from a map file have all issues of extended_meta fixed.
						std::string exp = "Meta file 0x" + to_hex_string(datum_index) + "." + type.as_string() + " is broken.\nEither debug the extraction proceedure or fix the meta file";
						throw new std::exception(exp.c_str());
					}

				}
			}
		}
		//now we go for withClass attribute tagRefs,they are a bit different as they only contain the datum index of the refered tag
		temp = fields->Get_WCtag_ref_list();
		for (int& i : temp)
		{
			int Toff = off + i;
			//we add this off to the list if it doesnt contain the off already
			if (find(ref_WCtags.begin(), ref_WCtags.end(), Toff) == ref_WCtags.end())
			{
				ref_WCtags.push_back(Toff);
			}
		}
	}
	/// use to rebase current meta data to a newer memory address(affects reflexive,extended and data ref fields only)
	/// <param name="new_base">the new memory address to which the meta has to rebased</param>
	void meta::Rebase_meta(int new_base)
	{
		//first rebase reflexive fields
		for(int& off : ref_reflexive)
		{
			int old_mem_addr = *(int*)(data + off + 4);  //DATA_READ.ReadINT_LE(off + 4, data);
			int new_mem_addr = new_base + (old_mem_addr - mem_off);

			*(int*)(data + off + 4) = new_mem_addr;
			//DATA_READ.WriteINT_LE(new_mem_addr, off + 4, data);
		}
		//then we rebase all the dataref fields
		for(int& off : ref_data)
		{
			int old_mem_addr = *(int*)(data + off + 4);//DATA_READ.ReadINT_LE(off + 4, data);
			int new_mem_addr = new_base + (old_mem_addr - mem_off);

			*(int*)(data + off + 4) = new_mem_addr;
			//DATA_READ.WriteINT_LE(new_mem_addr, off + 4, data);
		}
		//we venture in extended meta if it is even concerned,which is only valid when a mapstream is opened
		if (map_stream != nullptr)
		{
			//for extended meta stuff we are gonna first rebase the extended meta(s) first and set the offsets accordingly
			//well extende meta are gonna follow meta one by one
			int extended_new_base = new_base + size;

			std::list<int> key_mems;
			//map to list conversion
			for (auto& i : list_extended)
			{
				key_mems.push_back(i.first);
			}

			//Rebase extended meta
			for(int& temp_key : key_mems)
			{
				std::shared_ptr<meta> temp_meta = list_extended[temp_key];
				temp_meta->Rebase_meta(extended_new_base);

				extended_new_base += temp_meta->Get_Total_size();
			}

			//now lets update the offsets with the newer values
			std::list<int> extend_off;
			for (auto& i : ref_extended)
			{
				extend_off.push_back(i.first);
			}

			for (auto& temp_off : extend_off)
			{
				int extend_mem_addr = ref_extended[temp_off];
				std::shared_ptr<meta> temp_ext = list_extended[extend_mem_addr];

				int new_mem_addr = temp_ext->Get_mem_addr();

				*(int*)(data + temp_off + 0x4) = new_mem_addr;
			}
		}

		//update the base to which meta has been compiled
		mem_off = new_base;
	}
	/// returns the total size of the meta along with all extended meta sizes
	int meta::Get_Total_size()
	{
		int Tsize = size;

		if (map_stream != nullptr)
		{
			//lets add the extended meta sizes

			std::list<int> key_mems;
			for (auto& i : list_extended)
			{
				key_mems.push_back(i.first);
			}

			for (int& temp_key : key_mems)
			{
				std::shared_ptr<meta> temp_meta = list_extended[temp_key];
				Tsize += temp_meta->Get_Total_size();
			}
		}

		return Tsize;
	}
	//function that returns the mem_off the meta
	int meta::Get_mem_addr()
	{
		return mem_off;
	}
	///
	// Generates a full self dependent meta file(but doesnt fix tag refs)
	///
	char* meta::Generate_meta_file()
	{
		char* ret = new char[this->Get_Total_size()];

		//we first copy the root meta data into it
		memcpy(ret, data, this->size);  //DATA_READ.ArrayCpy(ret, data, 0x0, size);

		if (map_stream)
		{
			//now we go for extended meta

			std::list<int> extend_keys;
			for (auto& i : list_extended)
			{
				extend_keys.push_back(i.first);
			}
			//here we go
			for (int& temp_key : extend_keys)
			{
				std::shared_ptr<meta> temp_meta = list_extended[temp_key];
				int start_off = temp_meta->Get_mem_addr() - mem_off;

				char* temp_data = temp_meta->Generate_meta_file();

				memcpy(ret + start_off, temp_data, temp_meta->Get_Total_size());

				delete[] temp_data;
			}
		}

		return ret;
	}
	///
	//return a list all tagRefs mentioned in the meta and the extended meta
	///
	std::list<int> meta::Get_all_tag_refs()
	{
		std::list<int> ret;

		//first i add all my the tagRefs in the concerned meta
		for(int& temp_off : ref_tags)
		{
			int temp_datum = *(int*)(data + temp_off + 4);//DATA_READ.ReadINT_LE(temp_off + 4, data);

			//i only list them if they are valid
			if (temp_datum != -1)
				ret.push_back(temp_datum);
			
		}
		//list_extended object is only created when we are reading from a map
		if (map_stream != nullptr)
		{
			//then we add the extended_meta dependencies
			std::list<int> key_list;
			for (auto& i : list_extended)
			{
				key_list.push_back(i.first);
			}

			for(int& temp_key : key_list)
			{
				std::shared_ptr<meta> temp_meta = list_extended[temp_key];
				std::list<int> temp_tagref = temp_meta->Get_all_tag_refs();

				ret.insert(ret.cend(), temp_tagref.begin(), temp_tagref.end());

				//ret.AddRange(temp_meta->Get_all_tag_refs());
			}
		}
		//listing the WCtagRefs
		for(int& temp_off : ref_WCtags)
		{
			int temp_datum = *(int*)(data + temp_off);//DATA_READ.ReadINT_LE(temp_off, data);//we read it from meta data

			if (temp_datum != -1)
				ret.push_back(temp_datum);			
		}		

		return ret;
	}
	//
	// a function that updates the datum indexes acoording to the list supplied
	/// <returns>return a log about different encounters</returns>
	//
	std::string meta::Update_datum_indexes(std::list<injectRefs> tag_list)
	{
		std::string log = "\nUPDATE DATUM : " + to_hex_string(datum_index);

		//we loop through each offset

		//Updating TagRefs
		for(int& temp_off : ref_tags)
		{
			int temp_old_datum = *(int*)(data + temp_off + 4);//DATA_READ.ReadINT_LE(temp_off + 4, data);
			//next we loop through each list
			bool sucess = false;

			for(injectRefs& temp_ref : tag_list)
			{
				int old_datum = temp_ref.old_datum;
				int new_datum = temp_ref.new_datum;

				if (old_datum == temp_old_datum)
				{
					//we found a match
					if (!sucess)
					{

						*(int*)(data + temp_off + 4) = new_datum;//DATA_READ.WriteINT_LE(new_datum, temp_off + 4, data);
						log += "\nSuccesfully refered " + to_hex_string(temp_old_datum) + " to " + to_hex_string(new_datum);
						sucess = true;
					}
					else log += "\nMultiple occurences of old datum " + to_hex_string(old_datum) + " has been found";
				}
			}
			if (!sucess)
				log += "\nCouldnot find reference to " + to_hex_string(temp_old_datum);
			

		}
		//Updating WCtagRefs
		for(int& temp_off : ref_WCtags)
		{
			int temp_old_datum = *(int*)(data + temp_off);//DATA_READ.ReadINT_LE(temp_off, data);
			bool sucess = false;

			for(injectRefs& temp_ref : tag_list)
			{
				int old_datum = temp_ref.old_datum;
				int new_datum = temp_ref.new_datum;

				if (old_datum == temp_old_datum)
				{
					//we found a match
					if (!sucess)
					{
						*(int*)(data + temp_off) = new_datum;//DATA_READ.WriteINT_LE(new_datum, temp_off, data);
						log += "\nSuccesfully refered " + to_hex_string(temp_old_datum) + " to " + to_hex_string(new_datum);
						sucess = true;
					}
					else log += "\nMultiple occurences of old datum " + to_hex_string(old_datum) + " has been found";
				}
			}
			if (!sucess)
			{
				//Unfortunately WCtagRefs dont contain the type of tag to which they refer

				log += "\nCouldnot find reference to " + to_hex_string(temp_old_datum);
			}
		}

		return log;
	}
	///
	// A function that updates the StringIDS of the tag to match with the newer map
	// <returns>return info on various encounters</returns>
	///
	std::string meta::Update_StringID(std::list<StringIDRef> SID_list)
	{
		std::string log = "\nUPDATE StringID : " + to_hex_string(datum_index);

		for(int& temp_off : ref_stringID)
		{

			int temp_old_SID = *(int*)(data + temp_off);//DATA_READ.ReadINT_LE(temp_off, data);

			if (temp_old_SID != 0)
			{
				for(StringIDRef& temp_SID_ref : SID_list)
				{
					int old_SID = temp_SID_ref.old_SID;
					int new_SID = temp_SID_ref.new_SID;

					if (temp_old_SID == old_SID)
					{
						if (new_SID != -1)
						{
							*(int*)(data + temp_off) = new_SID;//DATA_READ.WriteINT_LE(new_SID, temp_off, data);
							log += "\nSuccesfully refered " + to_hex_string(temp_old_SID) + " to " + to_hex_string(new_SID);
							break;
						}
						else
						{
							log += "\nCouldnt refer " + to_hex_string(temp_old_SID);
							break;
						}
					}

				}
			}

		}

		return log;
	}
	//nulls all the stringIDs refered
	void meta::null_StringID()
	{
		for (int& temp_off : ref_stringID)
		{
			*(int*)(data + temp_off) = 0x0;//DATA_READ.WriteINT_LE(new_SID, temp_off, data);
		}
	}
	blam_tag meta::Get_type()
	{
		return type;
	}
	meta::~meta()
	{
		if (!NO_DELETE)
			delete[] data;
	}
	std::string meta::Get_map_loc()
	{
		return map_loc;
	}
	///
	//<---------------------------some usefull STRING functions------------------------------------------------->
	///
	std::string to_hex_string(int a)
	{
		char temp[15];

		sprintf(temp, "%X", a);

		std::string ret = temp;
		return ret;
	}
	std::string Get_file(std::string file_loc)
	{
		return file_loc.substr(file_loc.find_last_of('\\') + 1, file_loc.find_last_of('.') - file_loc.find_last_of('\\') - 1);
	}
	std::string Get_file_directory(std::string file_loc)
	{
		return file_loc.substr(0x0, file_loc.find_last_of('\\'));
	}
	std::string Get_file_type(std::string file)
	{
		return file.substr(file.find_last_of('.') + 1, file.length() - file.find_last_of('.') - 1);
	}
}
