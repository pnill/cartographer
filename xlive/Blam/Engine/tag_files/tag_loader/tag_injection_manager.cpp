#include "stdafx.h"
#include "tag_injection_manager.h"

#include "bitmaps/bitmap_group.h"
#include "geometry/geometry_block.h"
#include "models/render_model_definitions.h"
#include "render/weather_definitions.h"
#include "Util/filesys.h"

#define lazy_fread(FILE, OFFSET, OUT, SIZE, COUNT)\
	fseek(FILE, OFFSET, SEEK_SET);\
	fread(OUT, SIZE, COUNT, FILE)


c_tag_injecting_manager::c_tag_injecting_manager():
	m_agents{}, m_base_tag_data_size(0), m_injectable_used_size(0), m_instances(nullptr),
	m_active_map_verified(false), m_active_map_cache_header(),
	m_active_map_tags_header(), m_active_map_scenario_instance(),
	m_active_map_instance_table_offset(0), m_active_map_file_handle(nullptr)
{
	this->m_agents_initialized.clear();
}

void c_tag_injecting_manager::init_directories()
{
	this->m_base_map_directory.set(GetExeDirectoryNarrow().c_str());
	this->m_base_map_directory.append("\\maps\\");

	this->m_mods_map_directory.set(GetExeDirectoryNarrow().c_str());
	this->m_mods_map_directory.append("\\mods\\maps\\");

	this->m_plugins_directory.set(GetExeDirectoryNarrow().c_str());
	this->m_plugins_directory.append("\\mods\\plugins\\");
}

void c_tag_injecting_manager::set_base_map_tag_data_size(const uint32 size)
{
	this->m_base_tag_data_size = size;
}

void c_tag_injecting_manager::set_instance_table(tags::tag_instance* table)
{
	this->m_instances = table;
}

uint16 c_tag_injecting_manager::get_entry_count() const
{
	return this->m_table.get_entry_count();
}

c_tag_injection_table* c_tag_injecting_manager::get_table()
{
	return &this->m_table;
}

bool c_tag_injecting_manager::find_map(const char* map_name, c_static_string260* out_string)
{
	c_static_string260 test_path;
	test_path.set(this->m_base_map_directory.get_string());
	test_path.append(map_name);
	test_path.append(".map");
	if (PathFileExistsA(test_path.get_string()))
	{
		if (out_string)
			out_string->set(test_path.get_string());
		return true;
	}
	else
	{
		// Test if map exists in mods folder
		test_path.set(this->m_mods_map_directory.get_string());
		test_path.append(map_name);
		test_path.append(".map");
		if (PathFileExistsA(test_path.get_string()))
		{
			if (out_string)
				out_string->set(test_path.get_string());
			return true;
		}
		else
		{
			// if map is found in neither location then return the function early
			LOG_ERROR_GAME("[c_tag_injecting_manager::set_active_map] could not locate {}.map in any valid content location", map_name);
			return false;
		}
	}
}

void c_tag_injecting_manager::set_active_map(const char* map_name)
{
	this->m_active_map_verified = false;

	if (this->m_active_map_file_handle)
		fclose(this->m_active_map_file_handle);

	if (!this->find_map(map_name, &this->m_active_map))
		return;

	this->m_active_map_verified = true;
	this->m_active_map_file_handle = fopen(this->m_active_map.get_string(), "rb");

	// Read cache header from map file
	lazy_fread(this->m_active_map_file_handle, 0, &this->m_active_map_cache_header, sizeof(s_cache_header), 1);

	// Read tags header from map file
	lazy_fread(this->m_active_map_file_handle, this->m_active_map_cache_header.tag_offset, &this->m_active_map_tags_header, sizeof(s_tags_header), 1);

	this->m_active_map_instance_table_offset = this->m_active_map_cache_header.tag_offset + 0xC * this->m_active_map_tags_header.tag_parent_info_count + 0x20;

	// Read the scenario instance from map file
	lazy_fread(this->m_active_map_file_handle, m_active_map_instance_table_offset, &this->m_active_map_scenario_instance, sizeof(tags::tag_instance), 1);
}

bool c_tag_injecting_manager::get_active_map_verified()
{
	return this->m_active_map_verified;
}

void c_tag_injecting_manager::reset()
{
	// Set map verified too false, to prevent things from being loaded after a commit
	this->m_active_map_verified = false;

	// close the handle to the active map
	if (this->m_active_map_file_handle)
		fclose(this->m_active_map_file_handle);

	// just for safety clear the stored data for active map
	this->m_active_map.clear();
	memset(&this->m_active_map_cache_header, 0, sizeof(s_cache_header));
	memset(&this->m_active_map_tags_header, 0, sizeof(s_tags_header));
	memset(&this->m_active_map_scenario_instance, 0, sizeof(tags::tag_instance));

	// todo: clear entry table
	//this->m_table.clear();
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

void c_tag_injecting_manager::load_raw_data_from_cache(datum injected_index) const
{
	DWORD* PMapRawtableoffset = Memory::GetAddress<DWORD*>(0x4AE8B0);
	DWORD* PRawTableSize = Memory::GetAddress<DWORD*>(0x4AE8B4);

	//a little  precaution to circumvent unexpected behaviour
	DWORD oldRtable_offset = *PMapRawtableoffset;
	DWORD oldRtable_size = *PRawTableSize;

	*PMapRawtableoffset = 0x0;
	*PRawTableSize = 0x0;

	HANDLE old_file_handle = *Memory::GetAddress<HANDLE*>(0x4AE8A8);

	tags::tag_instance* tag_info = &this->m_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(injected_index)];
	char* tag_data = tags::get_tag_data() + this->m_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(injected_index)].data_offset;

	//fail safe
	if (DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_info->datum_index) != DATUM_INDEX_TO_ABSOLUTE_INDEX(injected_index))
	{
		LOG_ERROR_GAME("[tag_loader] failed to resolve datum to correct instance, game will crash");
	}

	//supposing full length
	HANDLE new_file_handle = CreateFileA(this->m_active_map.get_string(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		//(HANDLE)_get_osfhandle(_fileno(this->m_active_map_file_handle));
	
	*Memory::GetAddress<HANDLE*>(0x4AE8A8) = new_file_handle;

	switch (tag_info->type.group)
	{
	case 'mode':
	{
		render_model_definition* model_definition = (render_model_definition*)tag_data;
		if (model_definition->sections.count > 0)
		{
			int current_section_index = 0;
			do
			{
				render_model_section* model_section = model_definition->sections[current_section_index];

				((void(__cdecl*)(geometry_block_info*, unsigned int))Memory::GetAddress(0x2652BC))(&model_section->geometry_block_info, 3u);

				++current_section_index;
			} while (current_section_index < model_definition->sections.count);
		}
		break;
	}

	case 'bitm':
	{

		int old_list_field = *Memory::GetAddress<DWORD*>(0xA49270 + 0x1FC);
		bitmap_group* bitmap_definition = (bitmap_group*)tag_data;

		for (int i = 0; i < bitmap_definition->bitmaps.count; i++)
		{
			bitmap_data* bitmap_item = bitmap_definition->bitmaps[i];

			*Memory::GetAddress<bitmap_data**>(0xA49270 + 0x1FC) = bitmap_item;

			int temp = 0;
			((int(__cdecl*)(bitmap_data*, char, int, void*))Memory::GetAddress(0x265986))(bitmap_item, 2, 0, &temp);
			((int(__cdecl*)(bitmap_data*, char, int, void*))Memory::GetAddress(0x265986))(bitmap_item, 1, 0, &temp);
			((int(__cdecl*)(bitmap_data*, char, int, void*))Memory::GetAddress(0x265986))(bitmap_item, 0, 0, &temp);

		}
		*Memory::GetAddress<DWORD*>(0xA49270 + 0x1FC) = old_list_field;
		break;
	}
	case 'weat':
	{
		auto weather_tag = reinterpret_cast<c_weather_system*>(tag_data);
		for (auto i = 0; i < weather_tag->m_particle_system.count; i++)
			((void(__cdecl*)(long*, unsigned int))Memory::GetAddress(0x2652BC))(&weather_tag->m_particle_system[i]->m_geometry.block_offset, 3u);
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

void c_tag_injecting_manager::setup_havok_vtables(e_tag_group group, datum injected_index)
{
	switch(group)
	{
		case _tag_group_biped:
			{
				void(_cdecl * biped_post_load_init_havok)(datum) = (void(_cdecl*)(datum))Memory::GetAddress(0x1389B0);
				biped_post_load_init_havok(injected_index);
				break;
			}
		case _tag_group_creature:
			{
				void(_cdecl * creature_post_load_init_havok)(datum) = (void(_cdecl*)(datum))Memory::GetAddress(0x138985);
				creature_post_load_init_havok(injected_index);
				break;
			}
		case _tag_group_vehicle:
			{
				void(_cdecl * vehicle_post_load_init_havok)(datum) = (void(_cdecl*)(datum))Memory::GetAddress(0x13895A);
				vehicle_post_load_init_havok(injected_index);
				break;
			}
		case _tag_group_collision_model:
			{
				void(_cdecl * collision_post_load_init_havok)(datum) = (void(_cdecl*)(datum))Memory::GetAddress(0x7BE5C);
				collision_post_load_init_havok(injected_index);
				break;
			}
		case _tag_group_physics_model:
			{
				void(_cdecl * physics_model_post_load_init_havok)(datum, bool) = (void(_cdecl*)(datum, bool))Memory::GetAddress(0x7B844);
				physics_model_post_load_init_havok(injected_index, false);
				break;
			}
	}
}

void c_tag_injecting_manager::initialize_shader_template(datum injected_datum)
{
	typedef bool(__cdecl t_init_shader_template)(int a1);
	auto p_init_shader_template = Memory::GetAddress<t_init_shader_template*>(0x2694E6);
	tag_iterator* stem_iterator = Memory::GetAddress<tag_iterator*>(0xA4AF10);

	stem_iterator->next_tag_index = injected_datum;
	p_init_shader_template(1);
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

	if(tag_group_index == NONE)
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

datum c_tag_injecting_manager::load_tag(e_tag_group group, const char* tag_name, bool load_dependencies)
{
	datum cache_datum = this->get_tag_datum_by_name(group, tag_name);
	return this->load_tag(group, cache_datum, load_dependencies);
}

datum c_tag_injecting_manager::load_tag(e_tag_group group, datum cache_datum, bool load_dependencies)
{
	if (this->m_table.has_entry_by_cache_index(cache_datum))
		return NONE;

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

	return new_entry->injected_index;
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

void c_tag_injecting_manager::inject_tags()
{
	for(uint16 i = 0; i < this->m_table.get_entry_count(); i++)
	{
		s_tag_injecting_table_entry* entry = this->m_table.get_entry(i);

		if(entry->is_injected)
			continue;

		entry->is_injected = true;

		if(entry->type.group == _tag_group_bitmap)
		{
			bitmap_group* test =(bitmap_group*)entry->loaded_data.get_data();
			auto sdfsd = 213123;
		}

		uint32 injection_offset = this->m_base_tag_data_size + this->m_injectable_used_size;

		tags::tag_instance* injection_instance = &this->m_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(entry->injected_index)];

		injection_instance->type = entry->type;
		injection_instance->data_offset = injection_offset;
		injection_instance->size = entry->loaded_data.get_total_size();
		injection_instance->datum_index = entry->injected_index;

		entry->loaded_data.copy_tag_data((int8*)(tags::get_tag_data() + injection_offset), injection_offset);

		this->load_raw_data_from_cache(entry->injected_index);
		this->setup_havok_vtables(entry->type.group, entry->injected_index);

		if (entry->type.group == _tag_group_shader_template)
			this->initialize_shader_template(entry->injected_index);

		this->m_injectable_used_size += entry->loaded_data.get_total_size();
	}
}


#undef lazy_fread
