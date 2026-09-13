#include "stdafx.h"
#include "tag_injection_manager.h"

#include "tag_injection_define.h"

#include "bitmaps/bitmap_group.h"
#include "cache/cache_files.h"
#include "cache/pc_geometry_cache.h"
#include "cache/pc_texture_cache.h"
#include "creatures/creature_definitions.h"
#include "filesys/pc_file_system.h"
#include "game/game_globals.h"
#include "main/game_preferences.h"
#include "models/render_model_definitions.h"
#include "networking/network_event.h"
#include "physics/collision_model_definitions.h"
#include "physics/physics_model_definitions.h"
#include "render/render_weather.h"
#include "render/weather_definitions.h"
#include "scenario/scenario_definitions.h"
#include "shell/shell.h"
#include "tag_files/tag_loader/xml/xml_definition_loader.h"
#include "units/biped_definitions.h"
#include "units/vehicle_definitions.h"

extern bool g_force_cartographer_update;

/* constants */

#define k_relative_maps_directory L".\\maps\\"
#define k_relative_mods_directory L".\\mods\\"
#define k_relative_mods_maps_directory k_relative_mods_directory L"maps\\"
#define k_relative_mods_plugin_directory k_relative_mods_directory L"plugins\\"


c_tag_injecting_manager::c_tag_injecting_manager(void):
	m_agents{},
	m_base_tag_data_size(0),
	m_injectable_used_size(0),
	m_instances(nullptr),
	m_active_map_verified(false),
	m_active_map_cache_header(),
	m_active_map_tags_header(),
	m_active_map_scenario_instance_offset(),
	m_active_map_instance_table_offset(0),
	m_active_map_file_handle(nullptr)
{
	m_agents_initialized.clear();
	return;
}

void c_tag_injecting_manager::init_directories(void)
{
	m_base_map_directory.set(k_relative_maps_directory);
	m_mods_map_directory.set(k_relative_mods_maps_directory);
	m_plugins_directory.set(k_relative_mods_plugin_directory);
	return;
}

void c_tag_injecting_manager::set_base_map_tag_data_size(const uint32 size)
{
	m_base_tag_data_size = size;
}

uint32 c_tag_injecting_manager::get_base_map_tag_data_size(void) const
{
	return m_base_tag_data_size;
}

void c_tag_injecting_manager::set_instance_table(cache_file_tag_instance* table)
{
	m_instances = table;
	return;
}

uint16 c_tag_injecting_manager::get_entry_count(void) const
{
	return m_table.get_entry_count();
}

c_tag_injection_table* c_tag_injecting_manager::get_table(void)
{
	return &m_table;
}

bool c_tag_injecting_manager::find_map(const wchar_t* map_name, c_static_wchar_string<MAX_PATH>* out_string) const
{
	bool result = false;

	c_static_wchar_string<MAX_PATH> test_path;
	test_path.set(m_base_map_directory.get_string());
	test_path.append(map_name);
	test_path.append(L".map");
	
	if (PathFileExists(test_path.get_string()))
	{
		if (out_string)
		{
			out_string->set(test_path.get_string());
		}
		result = true;
	}
	else
	{
		// Test if map exists in mods folder
		test_path.set(m_mods_map_directory.get_string());
		test_path.append(map_name);
		test_path.append(L".map");
		if (PathFileExists(test_path.get_string()))
		{
			if (out_string)
			{
				out_string->set(test_path.get_string());
			}
			result = true;
		}
		// Exit and create a popup if a map is missing
		else
		{
			event(_event_error, "tags:injection: [%s] could not locate %ws.map in any valid content location", __FUNCTION__, map_name);
			g_force_cartographer_update = true;
		}
	}
	return result;
}

void c_tag_injecting_manager::set_active_map(const wchar_t* map_name)
{
	c_static_wchar_string<MAX_PATH> t_path;
	if (!find_map(map_name, &t_path))
		return;

	// if the requested map is already the active map of the loader no need to do any more work.
	if (t_path.is_equal(m_active_map.get_string()))
		return;

	m_active_map.set(t_path.get_string());

	m_active_map_verified = false;

	if (m_active_map_file_handle)
		fclose(m_active_map_file_handle);

	m_active_map_verified = true;
	
	// TODO: write out error
	m_active_map_file_handle = _wfsopen(m_active_map.get_string(), L"rb", SH_DENYNO);

	// Read cache header from map file
	file_seek_and_read(m_active_map_file_handle, 0, sizeof(cache_file_header), 1, &m_active_map_cache_header);

	// Read tags header from map file
	file_seek_and_read(m_active_map_file_handle, m_active_map_cache_header.tag_offset, sizeof(cache_file_tags_header), 1, &m_active_map_tags_header);


	m_active_map_instance_table_offset = m_active_map_cache_header.tag_offset + sizeof(s_tag_group_link) * m_active_map_tags_header.tag_group_link_set_count + sizeof(cache_file_tags_header);

	// Read the scenario instance from map file
	cache_file_tag_instance temp_instance;

	

	// fix for multiplayer shared cache type, where the tag table was intentionally made to be pushed farther down to prevent collisions.
	if (m_active_map_cache_header.type == _scenario_type_multiplayer_shared)
	{
		// The hired gun special
		m_active_map_scenario_instance_offset = m_active_map_cache_header.data_offset;
	}
	else
	{
		file_seek_and_read(m_active_map_file_handle, m_active_map_instance_table_offset, sizeof(cache_file_tag_instance), 1, &temp_instance);
		m_active_map_scenario_instance_offset = temp_instance.data_offset;
	}
	return;
}

bool c_tag_injecting_manager::get_active_map_verified(void) const
{
	return m_active_map_verified;
}

void c_tag_injecting_manager::close_active_map(void)
{
	// Set map verified too false, to prevent things from being loaded after a commit
	m_active_map_verified = false;

	// close the handle to the active map
	if (m_active_map_file_handle)
	{
		fclose(m_active_map_file_handle);
	}

	// just for safety clear the stored data for active map
	m_active_map.clear();

	return;
}

void c_tag_injecting_manager::reset(void)
{
	close_active_map();

	csmemset(&m_active_map_cache_header, 0, sizeof(cache_file_header));
	csmemset(&m_active_map_tags_header, 0, sizeof(cache_file_tags_header));
	m_active_map_scenario_instance_offset = 0;

	m_table.clear();

	m_injectable_used_size = 0;
	return;
}

cache_file_tag_instance c_tag_injecting_manager::get_tag_instance_from_cache(datum cache_datum) const
{
	cache_file_tag_instance temp_instance;
	uint32 instance_offset = m_active_map_instance_table_offset + sizeof(cache_file_tag_instance) * DATUM_INDEX_TO_ABSOLUTE_INDEX(cache_datum);
	file_seek_and_read(m_active_map_file_handle, instance_offset, sizeof(cache_file_tag_instance), 1, &temp_instance);
	return temp_instance;
}

tag_group c_tag_injecting_manager::get_tag_group_by_datum(datum cache_datum) const
{
	return get_tag_instance_from_cache(cache_datum).group_tag;
}

void c_tag_injecting_manager::load_raw_data_from_cache(datum injected_index) const
{
	// resource caches are disabled on H2Server
	if (shell_is_dedicated_server())
		return;

	// There is probably a struct here but can't identify anything
	HANDLE* g_cache_handle = Memory::GetAddress<HANDLE*>(0x4AE8A8, 0x4CF128);
	uint32* g_cache_handle_geometry_block_offset = Memory::GetAddress<DWORD*>(0x4AE8B0, 0x4CF130);
	uint32* g_cache_handle_geometry_block_size = Memory::GetAddress<DWORD*>(0x4AE8B4, 0x4CF134);

	// a little precaution to circumvent unexpected behaviour
	const uint32 previous_geometry_block_offset = *g_cache_handle_geometry_block_offset;
	const uint32 previous_geometry_block_size = *g_cache_handle_geometry_block_size;
	const HANDLE previous_cache_handle = *g_cache_handle;

	// Reset geometry block values to allow functions to reset them when called.
	*g_cache_handle_geometry_block_offset = 0x0;
	*g_cache_handle_geometry_block_size = 0x0;

	cache_file_tag_instance* tag_info = &m_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(injected_index)];
	uint8* tag_data = (uint8*)cache_get_tag_data() + m_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(injected_index)].data_offset;

	//fail safe
	if (DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_info->tag_index) != DATUM_INDEX_TO_ABSOLUTE_INDEX(injected_index))
	{
		error(_error_log, "[tag_loader] failed to resolve datum to correct instance, game will crash");
	}

#if TAG_INJECTION_DEBUG
	c_static_string<MAX_PATH> str;
	get_name_by_tag_datum(tag_info->group_tag.group, m_table.get_entry_by_injected_index(injected_index)->cache_index, str.get_buffer());
	event(_event_verbose, "tags:injection: [%s] loading %s index %x", __FUNCTION__, str.get_string(), injected_index);
#endif

	//supposing full length
	HANDLE new_file_handle = CreateFile(m_active_map.get_string(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	*g_cache_handle = new_file_handle;

	switch (tag_info->group_tag)
	{
		case _tag_group_render_model:
		{
			render_model_definition* model_definition = (render_model_definition*)tag_data;
			if (model_definition->sections.count > 0)
			{
				int current_section_index = 0;
				do
				{
					render_model_section* model_section = model_definition->sections[current_section_index];

					pc_geometry_cache_preload_geometry(&model_section->geometry_block_info, (e_pc_geometry_cache_preload_flags)(_pc_geometry_cache_preload_flag_2 | _pc_geometry_cache_preload_blocking));

					++current_section_index;
				} while (current_section_index < model_definition->sections.count);
			}
			break;
		}

		case _tag_group_bitmap:
		{
			int old_list_field = *Memory::GetAddress<DWORD*>(0xA49270 + 0x1FC);
			bitmap_group* bitmap_definition = (bitmap_group*)tag_data;

			for (int i = 0; i < bitmap_definition->bitmaps.count; i++)
			{
				bitmap_data* bitmap_item = bitmap_definition->bitmaps[i];

				*Memory::GetAddress<bitmap_data**>(0xA49270 + 0x1FC) = bitmap_item;

				pc_texture_cache_preload_bitmap(bitmap_item, 2, 0, nullptr);
				pc_texture_cache_preload_bitmap(bitmap_item, 1, 0, nullptr);
				pc_texture_cache_preload_bitmap(bitmap_item, 0, 0, nullptr);
			}
			*Memory::GetAddress<DWORD*>(0xA49270 + 0x1FC) = old_list_field;
			break;
		}
		case _tag_group_weather_system:
		{
			auto weather_tag = reinterpret_cast<c_weather_system*>(tag_data);
			for (auto i = 0; i < weather_tag->m_particle_system.count; i++)
			{
				c_particle_system_lite* system_block = TAG_BLOCK_GET_ELEMENT(&weather_tag->m_particle_system, i, c_particle_system_lite);

				pc_geometry_cache_preload_geometry(&system_block->m_geometry, (e_pc_geometry_cache_preload_flags)(_pc_geometry_cache_preload_flag_2 | _pc_geometry_cache_preload_blocking));
			}
			break;
	}
	default:
		break;
	}
	*g_cache_handle = previous_cache_handle;

	CloseHandle(new_file_handle);

	*g_cache_handle_geometry_block_offset = previous_geometry_block_offset;
	*g_cache_handle_geometry_block_size = previous_geometry_block_size;
	return;
}

void c_tag_injecting_manager::apply_definition_fixup(
	tag_group group,
	datum injected_index)
{
	switch(group)
	{
	case _tag_group_biped:
		biped_definitions_fixup(injected_index);
		break;
	case _tag_group_creature:
		creature_definitions_fixup(injected_index);
		break;
	case _tag_group_vehicle:
		vehicle_definitions_fixup(injected_index);
		break;
	case _tag_group_collision_model:
		collision_model_definitions_fixup(injected_index);
		break;
	case _tag_group_physics_model:
		physics_model_definitions_fixup(injected_index, false);
		break;
	default:
		break;
	}
	return;
}

void c_tag_injecting_manager::initialize_shader_template(datum injected_datum)
{
	if (shell_is_dedicated_server())
		return;

	typedef bool(__cdecl t_init_shader_template)(int a1);
	auto p_init_shader_template = Memory::GetAddress<t_init_shader_template*>(0x2694E6);
	tag_iterator* stem_iterator = Memory::GetAddress<tag_iterator*>(0xA4AF10);

	stem_iterator->next_tag_index = injected_datum;
	p_init_shader_template(1);
	return;
}

datum c_tag_injecting_manager::get_tag_datum_by_name(tag_group group, const char* tag_name) const
{
	if(!m_active_map_verified)
	{
		error(_error_log, "[%s] active map has not be set for tag: %s", __FUNCTION__, tag_name);
		return NONE;
	}
	
	const bool scenario_is_mp_shared = m_active_map_cache_header.type == _scenario_type_multiplayer_shared;

	// fix for multiplayer shared cache type, where the tag table was intentionally made to be pushed farther down to prevent collisions.
	const int32 start_index = scenario_is_mp_shared ? FIRST_SHARED_TAG_INSTANCE_INDEX : 0;

	datum result = NONE;
	for (int32 current_index = start_index; current_index < m_active_map_cache_header.debug_tag_name_count; ++current_index)
	{
		// Get the offset of the current index
		int32 current_offset;
		file_seek_and_read(m_active_map_file_handle, m_active_map_cache_header.tag_name_offset + sizeof(uint32) * current_index, sizeof(uint32), 1, &current_offset);

		uint32 current_size;
		if (current_index + 1 != m_active_map_cache_header.debug_tag_name_count)
		{
			// If the current offset is -1 it means we have reached the end of the index table
			if (current_offset == NONE)
			{
				break;
			}

			// Get the offset of the next index
			int32 next_offset;
			file_seek_and_read(m_active_map_file_handle, m_active_map_cache_header.tag_name_offset + sizeof(uint32) * (current_index + 1), sizeof(uint32), 1, &next_offset);

			// Current size is calculated using the offsets of the two indexes
			// if next offset is none, the current offset is the end of the table and just read max path
			current_size = next_offset == NONE ? MAX_PATH : next_offset - current_offset;
		}
		else
		{
			// Current size is calculated using the total size of the buffer and the current offset;
			current_size = (m_active_map_cache_header.tag_name_offset + (m_active_map_cache_header.debug_tag_name_count * sizeof(uint32))) - current_offset;
		}

		// Read the current debug name
		char name_buffer[MAX_PATH];
		file_seek_and_read(m_active_map_file_handle, m_active_map_cache_header.tag_name_buffer_offset + current_offset, current_size, 1, &name_buffer);

		if (csstricmp(tag_name, name_buffer) == 0)
		{
			cache_file_tag_instance temp_instance;
			file_seek_and_read(m_active_map_file_handle, m_active_map_instance_table_offset + (current_index * sizeof(cache_file_tag_instance)), sizeof(cache_file_tag_instance), 1, &temp_instance);
			if (temp_instance.group_tag == group)
			{
				result = temp_instance.tag_index;
				break;
			}
		}
	}

	return result;
}

void c_tag_injecting_manager::get_name_by_tag_datum(
	tag_group group,
	datum cache_datum, 
	char* out_name) const
{
	if (!m_active_map_verified)
	{
		error(_error_log, "[%s] active map has not be set for tag: %x", __FUNCTION__, cache_datum);
		out_name[0] = '\0';
		return;
	}

	const uint16 absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(cache_datum);

	cache_file_tag_instance temp_instance;
	file_seek_and_read(m_active_map_file_handle, m_active_map_instance_table_offset + (absolute_index * sizeof(cache_file_tag_instance)), sizeof(cache_file_tag_instance), 1, &temp_instance);

	if(temp_instance.tag_index != cache_datum || temp_instance.group_tag != group)
	{
		out_name[0] = '\0';
		return;
	}

	int32 current_offset = 0;
	uint32 next_offset = 0;
	uint32 current_size = 0;


	// Get the offset of the cache index
	file_seek_and_read(m_active_map_file_handle, m_active_map_cache_header.tag_name_offset + sizeof(uint32) * absolute_index, sizeof(uint32), 1, &current_offset);

	// If the current offset is NONE it means we have reached the end of the index table
	if (current_offset == NONE)
	{
		out_name[0] = '\0';
		return;
	}

	if (absolute_index + 1 != m_active_map_cache_header.debug_tag_name_count)
	{
		// Get the offset of the next index
		file_seek_and_read(m_active_map_file_handle, m_active_map_cache_header.tag_name_offset + sizeof(uint32) * (absolute_index + 1), sizeof(uint32), 1, &next_offset);

		// Current size is calculated using the offsets of the two indexes
		// if next offset is none, the current offset is the end of the table and just read max path
		current_size = (next_offset == NONE ? MAX_PATH : next_offset - current_offset);
	}
	else
	{
		// Current size is calculated using the total size of the buffer and the current offset;
		current_size = (m_active_map_cache_header.tag_name_offset + (m_active_map_cache_header.debug_tag_name_count * sizeof(uint32))) - current_offset;
	}

	// Read the current debug name
	file_seek_and_read(m_active_map_file_handle, m_active_map_cache_header.tag_name_buffer_offset + current_offset, current_size, 1, out_name);
	return;
}

bool c_tag_injecting_manager::initialize_agent(tag_group group)
{
	bool result = false;

	int32 tag_group_index = tag_group_get_as_index(group);
	
	const bool agent_initialized = m_agents_initialized.test(tag_group_index);	// check static flags if agent is already initialized
	if (tag_group_index != NONE && !agent_initialized)
	{
		char tag_class[5];
		tag_to_string(group, tag_class);

		// non-terminated sting correction
		tag_class[3] = (tag_class[3] == ' ' ? '\0' : tag_class[3]);

		wchar_t wide_tag_class[5];
		utf8_string_to_wchar_string(tag_class, wide_tag_class, NUMBEROF(wide_tag_class));

		c_static_wchar_string<MAX_PATH> plugin_path;
		plugin_path.set(m_plugins_directory.get_string());
		plugin_path.append(wide_tag_class);
		plugin_path.append(L".xml");

		// Exit and create a popup if a plugin is missing
		if (!PathFileExists(plugin_path.get_string()))
		{
			event(_event_error, "tags:injection: [%s] Plugin file could not be located %ws", __FUNCTION__, plugin_path.get_string());
			g_force_cartographer_update = true;
		}
		else
		{
			m_agents[tag_group_index].init(group, plugin_path.get_string());
			m_agents_initialized.set(tag_group_index, true);
			result = true;
		}
	}
	else
	{
		result = agent_initialized;
	}

	return result;
}

c_xml_definition_agent* c_tag_injecting_manager::get_agent(tag_group group)
{
	c_xml_definition_agent* result = nullptr;
	if (!initialize_agent(group))
	{
		char tag_class[5];
		tag_to_string(group, tag_class);

		// non-terminated sting correction
		tag_class[3] = (tag_class[3] == ' ' ? '\0' : tag_class[3]);


		error(_error_log, "%s: failed to initialize agent for %s", __FUNCTION__, tag_class);
	}
	else
	{
		result = &m_agents[tag_group_get_as_index(group)];
	}
	return result;
}

datum c_tag_injecting_manager::load_tag(
	tag_group group,
	char const* tag_name,
	bool load_dependencies)
{
	const datum cache_datum = get_tag_datum_by_name(group, tag_name);
	datum result = NONE;

	if (cache_datum != NONE)
	{
#if TAG_INJECTION_DEBUG
		event(_event_verbose, "tags:injection: [%s] loading %s with depencies %d datum %x", __FUNCTION__, tag_name, load_dependencies, cache_datum);
#endif
		result = load_tag(group, cache_datum, load_dependencies);
	}

	return result;
}

datum c_tag_injecting_manager::load_tag(
	tag_group group,
	datum cache_datum,
	bool load_dependencies)
{
	datum result = NONE;
	if (m_table.has_entry_by_cache_index(cache_datum))
	{
		result =  m_table.get_entry_by_cache_index(cache_datum)->injected_index;
	}
	else
	{
		const s_tag_injecting_table_entry* new_entry = m_table.init_entry(cache_datum, group);
		const c_xml_definition_agent* agent = get_agent(group);

		if (agent)
		{
			new_entry->loaded_data->init(
				agent->get_definition(),
				m_active_map_file_handle,
				&m_active_map_cache_header,
				&m_active_map_tags_header,
				m_active_map_scenario_instance_offset,
				cache_datum);

			if (load_dependencies)
			{
				c_tag_injecting_manager::load_dependencies(this, new_entry);
			}
			result = new_entry->injected_index;
		}
		// Force update if agent cannot be retrieved
		else
		{
			g_force_cartographer_update = true;
		}
	}

	return result;
}

void c_tag_injecting_manager::load_tag_internal(
	c_tag_injecting_manager* manager,
	tag_group group,
	datum cache_datum,
	bool load_dependencies)
{
	const cache_file_tag_instance inst = manager->get_tag_instance_from_cache(cache_datum);

	if (
		inst.tag_index == cache_datum && inst.group_tag == group &&
		inst.size != 0 && inst.data_offset != 0 && 
		!manager->m_table.has_entry_by_cache_index(cache_datum)
	)
	{
#if TAG_INJECTION_DEBUG
		c_static_string<MAX_PATH> name;
		manager->get_name_by_tag_datum(group.group, cache_datum, name.get_buffer());

		char tag_class[5];
		tag_class[0] = group.string[3];
		tag_class[1] = group.string[2];
		tag_class[2] = group.string[1];
		tag_class[3] = group.string[0];
		tag_class[4] = '\0';

		event(_event_verbose, "tags:injection: [%s] loading dependency %s %s", __FUNCTION__, name.get_string(), tag_class);
#endif

		const s_tag_injecting_table_entry* new_entry = manager->m_table.init_entry(cache_datum, group);
		const c_xml_definition_agent* agent = manager->get_agent(group);
		ASSERT(agent);
		new_entry->loaded_data->init(
			agent->get_definition(),
			manager->m_active_map_file_handle,
			&manager->m_active_map_cache_header,
			&manager->m_active_map_tags_header,
			manager->m_active_map_scenario_instance_offset,
			cache_datum
		);

		if (load_dependencies)
		{
#if TAG_INJECTION_DEBUG
			event(_event_verbose, "tags:injection: [%s] loading dependencies for %s %s", __FUNCTION__, name.get_string(), tag_class);
#endif
			c_tag_injecting_manager::load_dependencies(manager, new_entry);
		}
	}

	return;
}

void c_tag_injecting_manager::load_dependencies(c_tag_injecting_manager* manager, const s_tag_injecting_table_entry* new_entry)
{
	for (uint32 i = 0; i < new_entry->loaded_data->get_tag_reference_count(); ++i)
	{
		datum tag_index = new_entry->loaded_data->get_tag_reference(i);
		tag_group t_group = manager->get_tag_group_by_datum(tag_index);
		if (t_group != _tag_group_sound)
		{
			load_tag_internal(manager, t_group, tag_index, true);
		}
	}
	return;
}

void c_tag_injecting_manager::inject_tags(void)
{
#if TAG_INJECTION_DEBUG
	for (uint16 i = 0; i < m_table.get_entry_count(); i++)
	{
		s_tag_injecting_table_entry* entry = m_table.get_entry(i);
		char tag_class[5];
		tag_class[0] = entry->type.string[3];
		tag_class[1] = entry->type.string[2];
		tag_class[2] = entry->type.string[1];
		tag_class[3] = entry->type.string[0];
		tag_class[4] = '\0';
		c_static_string<MAX_PATH> tag_name;
		get_name_by_tag_datum(entry->type.group, entry->cache_index, tag_name.get_buffer());
		event(_event_verbose, "tags:injection: [%s]: cache_index: %x injected_index: %x type: %s tag_name: %s", __FUNCTION__, entry->cache_index, entry->injected_index, tag_class, tag_name.get_string());
	}
#endif
	// do the loading of the strings before writing the loaded tags into runtime tag cache so the remapped string reference indices match
	this->load_unicode_strings();

	for(uint16 i = 0; i < m_table.get_entry_count(); i++)
	{
		s_tag_injecting_table_entry* entry = m_table.get_entry(i);

		if(entry->is_injected)
			continue;

		entry->is_injected = true;

		uint32 injection_offset = m_base_tag_data_size + m_injectable_used_size;

#if TAG_INJECTION_DEBUG
		const uint32 start = (uint32)cache_get_tag_data();
		const uint32 end = start + get_base_map_tag_data_size() + k_injectable_allocation_size;
		bool in_range = (start + injection_offset) >= start && (start + injection_offset) < end;

		event(
			_event_verbose,
			"tags:injection: [%s] injection_offset: %x is valid: %d start: %x end: %x",
			__FUNCTION__, 
			(uint32)cache_get_tag_data() + injection_offset,
			in_range,
			start,
			end
		);
#endif


		cache_file_tag_instance* injection_instance = &m_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(entry->injected_index)];

		injection_instance->group_tag = entry->type;
		injection_instance->data_offset = injection_offset;
		injection_instance->size = entry->loaded_data->get_total_size();
		injection_instance->tag_index = entry->injected_index;

#if TAG_INJECTION_DEBUG
		{
			char tag_class[5];
			tag_class[0] = entry->type.string[3];
			tag_class[1] = entry->type.string[2];
			tag_class[2] = entry->type.string[1];
			tag_class[3] = entry->type.string[0];
			tag_class[4] = '\0';

			c_static_string<MAX_PATH> tag_name;
			get_name_by_tag_datum(entry->type.group, entry->cache_index, tag_name.get_buffer());

			event(_event_verbose, "tags:injection: [%s] type: %d injection_offset: %x data_size: %x tag_name: %s datum: %x", __FUNCTION__, tag_class, injection_offset, injection_instance->size, tag_name.get_string(), entry->injected_index);
		}
#endif

		entry->loaded_data->copy_tag_data((int8*)(cache_get_tag_data() + injection_offset), injection_offset);

		if (entry->type == _tag_group_bitmap || entry->type == _tag_group_render_model || entry->type == _tag_group_weather_system)
		{
			load_raw_data_from_cache(entry->injected_index);
		}

		apply_definition_fixup(entry->type, entry->injected_index);

		if (entry->type == _tag_group_shader_template)
			initialize_shader_template(entry->injected_index);

		char tag_name[MAX_PATH];
		get_name_by_tag_datum(entry->type, entry->cache_index, tag_name);

		tag_add_name(entry->injected_index, tag_name);

		m_injectable_used_size += entry->loaded_data->get_total_size();
	}
#if TAG_INJECTION_DEBUG
	event(_event_verbose, "tags:injection: [%s] Injection Complete", __FUNCTION__);
#endif
}

void* c_tag_injecting_manager::extend_tag_block(void* block, uint32 entry_size, uint32 count)
{
	s_tag_block* basic_block = (s_tag_block*)block;

	uint32 base_block_total_size = entry_size * basic_block->count;
	uint32 new_block_total_size = base_block_total_size + (entry_size * count);

	int8* base_block_location = (int8*)(cache_get_tag_data() + basic_block->data);

	uint32 injection_offset = m_base_tag_data_size + m_injectable_used_size;
	int8* injection_location = (int8*)(cache_get_tag_data() + injection_offset);

	csmemcpy(injection_location, base_block_location, base_block_total_size);

	basic_block->data = injection_offset;
	basic_block->count += count;

	m_injectable_used_size += new_block_total_size;

	// return the location in memory where the first newly added block exists
	return (void*)(cache_get_tag_data() + injection_offset + base_block_total_size);
}

void* c_tag_injecting_manager::reserve_space_in_cache_memory(uint32 size, uint32* out_data_offset)
{
	const uint32 reserved_offset = m_base_tag_data_size + m_injectable_used_size;

	m_injectable_used_size += size;

	*out_data_offset = reserved_offset;

	return (void*)(cache_get_tag_data() + reserved_offset);
}

void c_tag_injecting_manager::load_unicode_strings() const
{
	if (!this->m_active_map_verified)
		return;

	const uint16 unic_entry_count = this->m_table.get_entry_count_by_type(_tag_group_multilingual_unicode_string_list);

	if (!unic_entry_count)
		return;

	const e_language language = get_current_language();

	c_language_pack donor_pack{};

	// if the active map has a language pack table load that
	if (this->m_active_map_cache_header.language_pack_offset != NONE)
	{
		file_seek_and_read(
			this->m_active_map_file_handle,
			this->m_active_map_cache_header.language_pack_offset + sizeof(c_language_pack) * language,
			sizeof(c_language_pack),
			1,
			&donor_pack);
	}
	// if the active map doesn't have a language pack load the globals tag and read the language pack there
	else
	{
		s_game_globals map_globals{};

		datum map_globals_datum = this->get_tag_datum_by_name(_tag_group_globals, "globals\\globals");

		if (map_globals_datum == NONE)
			return;

		cache_file_tag_instance globals_instance = this->get_tag_instance_from_cache(map_globals_datum);

		if (globals_instance.size == 0)
			return;

		file_seek_and_read(this->m_active_map_file_handle, this->m_active_map_cache_header.tag_offset + globals_instance.data_offset, sizeof(s_game_globals), 1, &map_globals);

		donor_pack = map_globals.language_pack[language];
	}

	// the offset packs a few bits for something and checks it when loading if it's not 0 skip it.
	if (((uint32)donor_pack.get_string_reference_cache_offset() >> 30) != 0 || ((uint32)donor_pack.get_string_data_cache_offset() >> 30) != 0)
		return;

	if (donor_pack.get_number_of_strings() <= 0 || donor_pack.get_string_data_size() <= 0)
		return;

	const uint32 reference_cache_offset = (uint32)donor_pack.get_string_reference_cache_offset() & 0x3FFFFFFF;
	const uint32 string_data_cache_offset = (uint32)donor_pack.get_string_data_cache_offset() & 0x3FFFFFFF;

	// read each unic entry and read its reference block to get the totals needed for a combined buffer
	s_tag_injection_string_container* pending_string_item_buffer = (s_tag_injection_string_container*)calloc(unic_entry_count, sizeof(s_tag_injection_string_container));

	uint32 pending_count = 0;
	uint32 total_string_count = 0;
	uint32 total_buffer_length = 0;

	for (uint16 entry_index = 0; entry_index < this->m_table.get_entry_count(); ++entry_index)
	{
		s_tag_injecting_table_entry* unic_entry = this->m_table.get_entry(entry_index);

		if (unic_entry->type != _tag_group_multilingual_unicode_string_list)
			continue;

		if (unic_entry->is_post_processed)
			continue;

		// flag the entry so it can't be processed again
		unic_entry->is_post_processed = true;

		s_multilingual_unicode_string_list_group_header* unic_tag = (s_multilingual_unicode_string_list_group_header*)unic_entry->loaded_data->get_data();

		s_language_pack_offsets* unic_str = &unic_tag->langue_pack_offsets[language];

		if (unic_str->string_count == 0)
			continue;

		if (unic_str->start_index+ unic_str->string_count > donor_pack.get_number_of_strings())
		{
			event(_event_warning, "tags:injection: [%s] unic tag %08x string %d +%d exceeds language pack count %d skip",
				__FUNCTION__, unic_entry->cache_index, unic_str->start_index, unic_str->string_count, donor_pack.get_number_of_strings());
			continue;
		}

		// read the references block plus the one following it so the end of the last string is known without scanning for its terminator

		// check for if the reference block being read is at the end of the table
		const bool has_next_reference = unic_str->start_index + unic_str->string_count < donor_pack.get_number_of_strings();
		const uint32 read_count = unic_str->string_count + (has_next_reference ? 1 : 0);

		const uint32 reference_offset = reference_cache_offset + unic_str->start_index * sizeof(s_string_reference);

		s_string_reference* references = (s_string_reference*)calloc(read_count, sizeof(s_string_reference));

		file_seek_and_read(this->m_active_map_file_handle, reference_offset, sizeof(s_string_reference), read_count, references);

		const int32 first_string_offset = references[0].offset;
		const int32 end_string_offset = has_next_reference ? references[unic_str->string_count].offset : donor_pack.get_string_data_size();

		if (end_string_offset <= first_string_offset || end_string_offset > donor_pack.get_string_data_size())
		{
			event(_event_warning, "tags:injection: [%s] unic tag %08x invalid string range %x, %x, skipped",
				__FUNCTION__, unic_entry->cache_index, first_string_offset, end_string_offset);
			free(references);
			continue;
		}

		s_tag_injection_string_container* pending_string_item = &pending_string_item_buffer[pending_count++];

		pending_string_item->unic_str = unic_str;
		pending_string_item->references = references;
		pending_string_item->strings_count = unic_str->string_count;
		pending_string_item->first_string_offset = first_string_offset;

		pending_string_item->buffer_length = end_string_offset - first_string_offset;

		pending_string_item->base_index = total_string_count;

		total_string_count += pending_string_item->strings_count;
		total_buffer_length += pending_string_item->buffer_length;
	}

	// build one combined reference table and string buffer append them to the runtime
	// language pack in a single call and distribute the returned base index back to the tags
	if (total_string_count)
	{
		s_string_reference* merged_reference_buffer = (s_string_reference*)calloc(total_string_count, sizeof(s_string_reference));
		utf8* merged_string_buffer = (utf8*)calloc(1, total_buffer_length + 1);

		uint32 current_buffer_offset = 0;

		for (uint32 pending_index = 0; pending_index < pending_count; ++pending_index)
		{
			s_tag_injection_string_container* pending_string_item = &pending_string_item_buffer[pending_index];

			file_seek_and_read(this->m_active_map_file_handle, string_data_cache_offset + pending_string_item->first_string_offset, pending_string_item->buffer_length, 1, merged_string_buffer + current_buffer_offset);

			// rebase the references relative to the combined buffer
			// pending_item->base_index as the starting location for the current reference block
			for (uint32 str_index = 0; str_index < pending_string_item->strings_count; ++str_index)
			{
				merged_reference_buffer[pending_string_item->base_index + str_index].string_id = pending_string_item->references[str_index].string_id;
				merged_reference_buffer[pending_string_item->base_index + str_index].offset = (pending_string_item->references[str_index].offset - pending_string_item->first_string_offset) + current_buffer_offset;
			}

			current_buffer_offset += pending_string_item->buffer_length;
		}

		merged_string_buffer[total_buffer_length] = '\0';

		uint16 base_index = 0;

		language_pack_get()->append_strings(merged_reference_buffer, merged_string_buffer, total_buffer_length, total_string_count, &base_index);

		// remap each loaded tag to its new base index
		for (uint32 pending_index = 0; pending_index < pending_count; ++pending_index)
		{
			pending_string_item_buffer[pending_index].unic_str->start_index = base_index + (uint16)pending_string_item_buffer[pending_index].base_index;
		}

		free(merged_reference_buffer);
		free(merged_string_buffer);
	}

	for (uint32 pending_index = 0; pending_index < pending_count; ++pending_index)
		free(pending_string_item_buffer[pending_index].references);

	free(pending_string_item_buffer);
}
