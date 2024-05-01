#include "stdafx.h"
#include "tag_injection_manager.h"

#include "Util/filesys.h"

#define lazy_fread(FILE, OFFSET, OUT, SIZE, COUNT)\
	fseek(FILE, OFFSET, SEEK_SET);\
	fread(OUT, SIZE, COUNT, FILE)


c_tag_injecting_manager::c_tag_injecting_manager():
	m_agents{}, m_active_map_verified(false), m_active_map_cache_header(),
	m_active_map_tags_header(), m_active_map_scenario_instance(),
	m_active_map_file_handle(nullptr)
{
	this->m_agents_initialized.clear();
	this->m_base_map_directory.set(GetExeDirectoryNarrow().c_str());
	this->m_base_map_directory.append("\\maps\\");

	this->m_mods_map_directory.set(GetExeDirectoryNarrow().c_str());
	this->m_mods_map_directory.append("\\mods\\maps\\");

	this->m_plugins_directory.set(GetExeDirectoryNarrow().c_str());
	this->m_plugins_directory.append("\\mods\\plugins\\");
}

void c_tag_injecting_manager::set_active_map(const char* map_name)
{
	this->m_active_map_verified = false;
	c_static_string260 test_path;

	// Test if map exists in base maps
	test_path.set(this->m_base_map_directory.get_string());
	test_path.append(map_name);
	test_path.append(".map");
	if(PathFileExistsA(test_path.get_string()))
	{
		this->m_active_map.set(test_path.get_string());
	}
	else
	{
		// Test if map exists in mods folder
		test_path.set(this->m_mods_map_directory.get_string());
		test_path.append(map_name);
		test_path.append(".map");
		if (PathFileExistsA(test_path.get_string()))
		{
			this->m_active_map.set(test_path.get_string());
		}
		else
		{
			// if map is found in neither location then return the function early
			LOG_ERROR_GAME("[c_tag_injecting_manager::set_active_map] could not locate {}.map in any valid content location", map_name);
			return;
		}
	}
	this->m_active_map_verified = true;
	this->m_active_map_file_handle = fopen(this->m_active_map.get_string(), "r");

	// Read cache header from map file
	lazy_fread(this->m_active_map_file_handle, 0, &this->m_active_map_cache_header, sizeof(s_cache_header), 1);

	// Read tags header from map file
	lazy_fread(this->m_active_map_file_handle, this->m_active_map_cache_header.tag_offset, &this->m_active_map_tags_header, sizeof(s_tags_header), 1);

	this->m_active_map_instance_table_offset = this->m_active_map_cache_header.tag_offset + 0xC * this->m_active_map_tags_header.tag_parent_info_count + 0x20;

	// Read the scenario instance from map file
	lazy_fread(this->m_active_map_file_handle, m_active_map_instance_table_offset, &this->m_active_map_scenario_instance, sizeof(tags::tag_instance), 1);
}

void c_tag_injecting_manager::reset()
{
	// Set map verified too false, to prevent things from being loaded after a commit
	this->m_active_map_verified = false;

	// close the handle to the active map
	fclose(this->m_active_map_file_handle);

	// just for safety clear the stored data for active map
	this->m_active_map.clear();
	memset(&this->m_active_map_cache_header, 0, sizeof(s_cache_header));
	memset(&this->m_active_map_tags_header, 0, sizeof(s_tags_header));
	memset(&this->m_active_map_scenario_instance, 0, sizeof(tags::tag_instance));
}



tags::tag_instance c_tag_injecting_manager::get_tag_instance_from_cache(datum cache_datum) const
{
	tags::tag_instance temp_instance;
	uint32 instance_offset = this->m_active_map_instance_table_offset + sizeof(tags::tag_instance) * DATUM_INDEX_TO_ABSOLUTE_INDEX(cache_datum);
	lazy_fread(this->m_active_map_file_handle, instance_offset, &temp_instance, sizeof(tags::tag_instance), 1);
	return temp_instance;
}

tag_group c_tag_injecting_manager::get_tag_group_by_datum(datum cache_datum) const
{
	return this->get_tag_instance_from_cache(cache_datum).type;
}

datum c_tag_injecting_manager::get_tag_datum_by_name(e_tag_group group, const char* tag_name) const
{
	if(!this->m_active_map_verified)
	{
		LOG_ERROR_GAME("[c_tag_injecting_mananger::get_tag_datum_by_name] active map has not be set for tag: {}", tag_name);
		return NONE;
	}
	
	tags::tag_instance temp_instance;

	fseek(this->m_active_map_file_handle, this->m_active_map_cache_header.tag_name_buffer_offset, SEEK_SET);
	uint32 current_index = 0;
	uint32 chars_read = 0;
	uint16 buff_size = 0;
	char buff[MAX_PATH];

	while(true)
	{
		char c = (char)fgetc(this->m_active_map_file_handle);
		chars_read++;
		if (c == '\0')
		{
			if (memcmp(tag_name, buff, strlen(tag_name)) == 0)
			{
				lazy_fread(this->m_active_map_file_handle, this->m_active_map_instance_table_offset + (current_index * sizeof(tags::tag_instance)), &temp_instance, sizeof(tags::tag_instance), 1);
				if (temp_instance.type.group == group)
					return temp_instance.datum_index;
			}
			current_index++;
			buff_size = 0;
		}
		else
		{
			buff[buff_size] = c;
			buff[buff_size + 1] = '\0';
			buff_size++;
		}
		if (chars_read > this->m_active_map_cache_header.tag_name_buffer_size)
			break;
	}
	return NONE;
}

void c_tag_injecting_manager::get_name_by_tag_datum(e_tag_group group, datum cache_datum, char* out_name) const
{
	if (!this->m_active_map_verified)
	{
		LOG_ERROR_GAME("[c_tag_injecting_mananger::get_name_by_tag_datum] active map has not be set for tag: {:x}", cache_datum);
	}

	tags::tag_instance temp_instance;

	fseek(this->m_active_map_file_handle, this->m_active_map_cache_header.tag_name_buffer_offset, SEEK_SET);
	uint32 current_index = 0;
	uint32 chars_read = 0;
	uint16 buff_size = 0;
	char buff[MAX_PATH];

	while (true)
	{
		char c = (char)fgetc(this->m_active_map_file_handle);
		chars_read++;
		if (c == '\0')
		{
			fpos_t position;
			fgetpos(this->m_active_map_file_handle, &position);
			lazy_fread(this->m_active_map_file_handle, this->m_active_map_instance_table_offset + (current_index * sizeof(tags::tag_instance)), &temp_instance, sizeof(tags::tag_instance), 1);
			if (temp_instance.datum_index == cache_datum)
			{
				if (temp_instance.type.group == group)
					memcpy(out_name, buff, MAX_PATH);
			}
			fseek(this->m_active_map_file_handle, position, SEEK_SET);
			current_index++;
			buff_size = 0;
		}
		else
		{
			buff[buff_size] = c;
			buff[buff_size + 1] = '\0';
			buff_size++;
		}
		if (chars_read > this->m_active_map_cache_header.tag_name_buffer_size)
			break;
	}
}

bool c_tag_injecting_manager::initialize_agent(tag_group group)
{
	uint32 tag_group_index = tag_group_get_as_index(group);

	if(tag_group_index == -1)
	{
		return false;
	}

	// check static flags if agent is already initialized
	if (this->m_agents_initialized.test(tag_group_index))
		return true;

	// non-terminated sting correction
	// flip string and terminate
	char null_terminated_class[5];
	null_terminated_class[0] = group.string[3];
	null_terminated_class[1] = group.string[2];
	null_terminated_class[2] = group.string[1];
	null_terminated_class[3] = group.string[0];
	null_terminated_class[4] = '\0';

	c_static_string260 plugin_path;
	plugin_path.set(this->m_plugins_directory.get_string());
	plugin_path.append(null_terminated_class);
	plugin_path.append(".xml");

	if (!PathFileExistsA(plugin_path.get_string()))
	{
		LOG_ERROR_GAME("[c_tag_injecting_manager::initialize_agent] Plugin file could not be located {}", plugin_path.get_string());
		return false;
	}
	this->m_agents[tag_group_index].init(group, plugin_path.get_string());
	this->m_agents_initialized.set(tag_group_index, true);
	return true;
}

c_xml_definition_agent* c_tag_injecting_manager::get_agent(tag_group group)
{
	if(!this->initialize_agent(group))
	{
		// non-terminated sting correction
		// flip string and terminate
		char null_terminated_class[5];
		null_terminated_class[0] = group.string[3];
		null_terminated_class[1] = group.string[2];
		null_terminated_class[2] = group.string[1];
		null_terminated_class[3] = group.string[0];
		null_terminated_class[4] = '\0';

		LOG_ERROR_GAME("[c_tag_injecting_manager::get_agent] failed to initialize agent for {}", null_terminated_class);
		return nullptr;
	}
	return &this->m_agents[tag_group_get_as_index(group)];
}

void c_tag_injecting_manager::load_tag(e_tag_group group, const char* tag_name, bool load_dependencies)
{
	datum cache_datum = this->get_tag_datum_by_name(group, tag_name);

	LOG_INFO_GAME("[c_tag_injection_manager::load_tag] loading {}", tag_name);

	if (this->m_table.has_entry_by_cache_index(cache_datum))
		return;

	s_tag_injecting_table_entry* new_entry = this->m_table.init_entry(cache_datum, group);

	c_xml_definition_agent* agent = this->get_agent(tag_group_from_enum(group));
	new_entry->loaded_data.init(
		agent->get_definition(),
		this->m_active_map_file_handle,
		&this->m_active_map_cache_header,
		&this->m_active_map_tags_header,
		&this->m_active_map_scenario_instance,
		cache_datum);

	if (load_dependencies)
	{
		for (uint32 i = 0; i < new_entry->loaded_data.m_tag_reference_count; i++)
		{
			tag_group t_group = this->get_tag_group_by_datum(new_entry->loaded_data.m_tag_references[i]);
			load_tag_internal(this, t_group, new_entry->loaded_data.m_tag_references[i], load_dependencies);
		}
	}
}

void c_tag_injecting_manager::load_tag_internal(c_tag_injecting_manager* manager, tag_group group, datum cache_datum,
	bool load_dependencies)
{
	tags::tag_instance inst = manager->get_tag_instance_from_cache(cache_datum);
	if (inst.datum_index != cache_datum || inst.type.group != group.group)
		return;

	if (inst.size == 0 || inst.data_offset == 0)
		return;

	if (manager->m_table.has_entry_by_cache_index(cache_datum))
		return;

	c_static_string260 name;
	manager->get_name_by_tag_datum(group.group, cache_datum, name.get_buffer());

	LOG_INFO_GAME("[c_tag_injection_manager::load_tag] loading dependency {}", name.get_string());

	s_tag_injecting_table_entry* new_entry = manager->m_table.init_entry(cache_datum, group.group);
	c_xml_definition_agent* agent = manager->get_agent(group);
	new_entry->loaded_data.init(
		agent->get_definition(),
		manager->m_active_map_file_handle,
		&manager->m_active_map_cache_header,
		&manager->m_active_map_tags_header,
		&manager->m_active_map_scenario_instance,
		cache_datum);

	if (load_dependencies)
	{
		for (uint32 i = 0; i < new_entry->loaded_data.m_tag_reference_count; i++)
		{
			tag_group t_group = manager->get_tag_group_by_datum(new_entry->loaded_data.m_tag_references[i]);
			load_tag_internal(manager, t_group, new_entry->loaded_data.m_tag_references[i], load_dependencies);
		}
	}
}


#undef lazy_fread
