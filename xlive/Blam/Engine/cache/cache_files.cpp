#include "stdafx.h"
#include "cache_files.h"

#include "cseries/cseries_system_memory.h"
#include "game/game_globals.h"
#include "main/game_preferences.h"
#include "memory/data.h"
#include "networking/network_event.h"
#include "scenario/scenario.h"
#include "shell/shell.h"
#include "sound/sound_cache_file_definitions.h"
#include "tag_files/tag_loader/tag_injection.h"
#include "tag_files/tag_loader/tag_injection_manager.h"
#include "tag_files/tag_group_access.h"

/* typedefs */

typedef void* (__cdecl* t_tag_get_safe)(tag_group group, datum tag_index);

/* globals */

// H2V does not include these parameters inside the s_cache_file_memory_globals struct, so we have to create and store them elsewhere
s_cache_file_debug_globals g_cache_file_debug_globals;

t_tag_get_safe p_tag_get_safe;

/* prototypes */

bool __cdecl read_shared_resource_database(e_shared_resource_database_type database_type, int32 unused_flags, uint32 offset, uint32 size, void* out_buffer, bool async);
void* tag_get_safe(tag_group group, datum tag_index);

/* public code */

void cache_files_apply_patches(void)
{
	// Default Maps
	PatchCall(Memory::GetAddress(0x3166B, 0x2551B), scenario_tags_load_internal);
	// Custom Maps
	PatchCall(Memory::GetAddress(0x315ED, 0x2549D), scenario_tags_load_internal);

	DETOUR_ATTACH(p_tag_get_safe, Memory::GetAddress<t_tag_get_safe>(0x316C3, 0x25573), tag_get_safe);
	return;
}

void* cache_file_handle_get(void)
{
	return Memory::GetAddress<HANDLE*>(0x4AE8A8, 0x4CF128);
}

uintptr_t* tag_data_get(void)
{
	return Memory::GetAddress<uintptr_t*>(0x482290, 0x4A6438);
}

s_cache_file_memory_globals* cache_file_memory_globals_get(void)
{
	return Memory::GetAddress<s_cache_file_memory_globals*>(0x47CD60, 0x4A29C8);
}

void* tag_data_get_from_instance(
	cache_file_tag_instance const* tag_instance)
{
	ASSERT(tag_instance);

	return
		tag_instance->data_offset != NONE ?
		(void*)(tag_instance->data_offset + *tag_data_get()) :
		NULL;
}

bool cache_file_is_loaded(void)
{
	const s_cache_file_memory_globals* cache_file_memory_globals = cache_file_memory_globals_get();

	return cache_file_memory_globals->tags_loaded;
}

cache_file_header* cache_files_get_header(void)
{
	s_cache_file_memory_globals* cache_file_memory_globals = cache_file_memory_globals_get();

	return &cache_file_memory_globals->header;
}

cache_file_tags_header* cache_files_get_tags_header(void)
{
	const s_cache_file_memory_globals* cache_file_memory_globals = cache_file_memory_globals_get();

	return cache_file_memory_globals->tags_header;
}

cache_file_tag_instance* global_tag_instances_get(void)
{
	const s_cache_file_memory_globals* cache_file_memory_globals = cache_file_memory_globals_get();

	return cache_file_memory_globals->tags_header->tag_instances;
}

int8* cache_get_tag_data(uint32 offset)
{
	const s_cache_file_memory_globals* cache_file_memory_globals = cache_file_memory_globals_get();

	return (int8*)(cache_file_memory_globals->tag_cache_base_address + offset);
}

cache_file_tag_instance* cache_get_tag_instance(datum tag_index)
{
	const s_cache_file_memory_globals* cache_file_memory_globals = cache_file_memory_globals_get();

	return &cache_file_memory_globals->tags_header->tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_index)];
}

tag_iterator* tag_iterator_new(tag_iterator* itr, e_tag_group type)
{
	itr->next_tag_index = 0;
	itr->tag_type = type;
	return itr;
}

datum __cdecl tag_iterator_next(tag_iterator* itr)
{
	return INVOKE(0x3172C, 0x255DC, tag_iterator_next, itr);
}

void __cdecl cache_file_map_clear_all_failures(void)
{
	INVOKE(0x64551, 0x4C5A8, cache_file_map_clear_all_failures);
	return;
}

void* __cdecl tag_get_fast(datum tag_index)
{
	const s_cache_file_memory_globals* cache_file_memory_globals = cache_file_memory_globals_get();
	return (void*)(cache_file_memory_globals->tag_cache_base_address + 	cache_get_tag_instance(tag_index)->data_offset);
}

void __cdecl cache_file_close()
{
	INVOKE(0x64C37, 0x4CC8E, cache_file_close);
}

bool __cdecl cache_header_verify(cache_file_header* cache_header)
{
	return INVOKE(0x310E9, 0x24F99, cache_header_verify, cache_header);
}

uint32 __cdecl cache_file_align_read_size_to_cache_page(uint32 size)
{
	return INVOKE(0x647DA, 0x4C831, cache_file_align_read_size_to_cache_page, size);
}

bool __cdecl cache_file_blocking_read(intptr_t a1, uint32 cache_offset, uint32 read_size, void* out_buffer)
{
	return INVOKE(0x64D01, 0x4CD58, cache_file_blocking_read, a1, cache_offset, read_size, out_buffer);
}

bool scenario_tags_load_process_shared_tags()
{
	s_cache_file_memory_globals* cache_file_memory = cache_file_memory_globals_get();

	cache_file_tags_header* tag_header = cache_file_memory->tags_header;
	cache_file_tags_header* unmasked_tag_header = (cache_file_tags_header*)cache_file_memory->tag_cache_base_address;
	cache_file_header* cache_header = cache_files_get_header();
	cache_file_header shared_header;

	csmemset(&shared_header, 0, sizeof(cache_file_header));

	// Read cache header
	if(!read_shared_resource_database(_shared_resource_database_type_multi_player, NONE, 0, sizeof(cache_file_header), &shared_header, false))
		return false;

	const uint32 aligned_tag_size = cache_file_align_read_size_to_cache_page(shared_header.tag_size);
	const uint32 aligned_data_offset = cache_file_align_read_size_to_cache_page(shared_header.data_offset);
	const uint32 aligned_data_size = cache_file_align_read_size_to_cache_page(shared_header.data_size);

	if (aligned_tag_size > (uint32)cache_header->tag_offset_mask)
		return false;

	// Read tags header
	if (!read_shared_resource_database(_shared_resource_database_type_multi_player, NONE, shared_header.tag_offset, aligned_data_offset, unmasked_tag_header, false))
		return false;

	if (unmasked_tag_header->tag_count < FIRST_SHARED_TAG_INSTANCE_INDEX)
		return false;

	int8* shared_data_start = (int8*)(cache_file_memory->tag_cache_base_address + aligned_tag_size - shared_header.data_size);

	// Read tag data
	if (!read_shared_resource_database(_shared_resource_database_type_multi_player, NONE, shared_header.data_offset + shared_header.tag_offset, aligned_data_size, shared_data_start, false))
		return false;

	unmasked_tag_header->tag_group_link_set = (s_tag_group_link*)&unmasked_tag_header[1];
	unmasked_tag_header->tag_instances = (cache_file_tag_instance*)(((char*)&unmasked_tag_header[1]) + sizeof(s_tag_group_link) * unmasked_tag_header->tag_group_link_set_count);

	if(tag_header->tag_count >= FIRST_SHARED_TAG_INSTANCE_INDEX)
	{
		// Update cache tag_header's instances referencing shared tags
		int32 absolute_index = FIRST_SHARED_TAG_INSTANCE_INDEX;
		for (; absolute_index < tag_header->tag_count; absolute_index++)
		{
			if(tag_header->tag_instances[absolute_index].tag_index != NONE)
			{
				tag_header->tag_instances[absolute_index].data_offset = unmasked_tag_header->tag_instances[absolute_index].data_offset;
				tag_header->tag_instances[absolute_index].size = unmasked_tag_header->tag_instances[absolute_index].size;
			}
		}
	}

	return true;
}

bool __cdecl scenario_tags_load_internal(const char* scenario_path)
{
	int32 tag_index = NONE;

	cache_file_header* cache_header = cache_files_get_header();
	s_cache_file_memory_globals* cache_file_memory_globals = cache_file_memory_globals_get();

	const bool custom_map = cache_file_memory_globals->custom_map;
	bool is_compatible = false;
	
	if (cache_header_verify(cache_header) && csstrnlen(cache_header->version_string, NUMBEROF(cache_header->version_string)) < 32)
	{
		if (shell_build_string_is_compatible(cache_header->version_string))
		{
			is_compatible = true;
		}
		else if (!custom_map)
		{
			error(_error_immediate, "the cache file '%s' belongs to an incompatible build (%s)", cache_file_memory_globals->header.name, cache_file_memory_globals->header.version_string);
		}
	}

	if (!is_compatible)
	{
		goto cache_file_header_invalid;
	}

	cache_header->tag_size = cache_file_align_read_size_to_cache_page(cache_header->tag_size);

	const uint32 aligned_tag_size_read = cache_header->tag_size + cache_header->tag_offset_mask;
	cache_file_memory_globals->tag_cache_base_address = datum_header_allocate(aligned_tag_size_read + k_injectable_allocation_size, 12);

	if(!cache_file_memory_globals->tag_cache_base_address)
	{
		error(_error_immediate, "failed to allocate the physical memory for the tags");
cache_file_header_invalid:
		error(_error_immediate, "cache file header is invalid");
		goto scenario_tags_load_internal_end;
	}

	csmemset((void*)cache_file_memory_globals->tag_cache_base_address, 0, aligned_tag_size_read);
	tag_group_set_data_info(cache_file_memory_globals->tag_cache_base_address, aligned_tag_size_read);

	const uint32 aligned_tag_header_read_size = cache_file_align_read_size_to_cache_page(cache_header->data_offset);
	const uint32 aligned_tag_data_read_size = cache_file_align_read_size_to_cache_page(cache_header->data_size);

	// Read tag header data

	void* memory_tag_header_data_start = (void*)(cache_file_memory_globals->tag_cache_base_address + cache_header->tag_offset_mask);
	is_compatible = cache_file_blocking_read(NONE, cache_header->tag_offset, aligned_tag_header_read_size, memory_tag_header_data_start);
	if (!is_compatible)
	{
		global_preferences_flag_dirty();
		error(_error_immediate, "failed to read the tags header, instances, and names section");
		event(_event_error, "failed to read the tags header, instances, and names section [%s]", scenario_path);
		goto cache_file_header_invalid;
	}

	// Read tag data

	void* memory_tag_data_start = (void*)(cache_header->tag_size + cache_header->tag_offset_mask + cache_file_memory_globals->tag_cache_base_address - cache_header->data_size);
	is_compatible = cache_file_blocking_read(NONE, cache_header->data_offset + cache_header->tag_offset, aligned_tag_data_read_size, memory_tag_data_start);
	if(!is_compatible)
	{
		global_preferences_flag_dirty();
		error(_error_immediate, "failed to read the tag data section");
		event(_event_error, "failed to read the tag data section [%s]", scenario_path);
		goto cache_file_header_invalid;
	}

	cache_file_tags_header* tag_header = (cache_file_tags_header*)memory_tag_header_data_start;

	if(!tag_header->tag_instances || tag_header->tag_count <= 0 || tag_header->signature != 'tags')
	{
		global_preferences_flag_dirty();
		error(_error_immediate, "tag header is invalid");
		event(_event_error, "tag header is invalid [%s]", scenario_path);
		goto cache_file_header_invalid;
	}

	cache_file_memory_globals->tags_header = tag_header;

	is_compatible = string_id_load_strings(&cache_file_memory_globals->header);
	if (!is_compatible)
	{
		event(_event_error, "failed to load the string ids [%s]", scenario_path);
		goto cache_file_header_invalid;
	}

	// Read tag names

	const uint32 aligned_tag_name_read_size = cache_file_align_read_size_to_cache_page(cache_file_memory_globals->header.tag_name_buffer_size);
	ASSERT(aligned_tag_name_read_size <= sizeof(g_cache_file_debug_globals.debug_tag_name_buffer));

	is_compatible = cache_file_blocking_read(
		NONE,
		cache_file_memory_globals->header.tag_name_buffer_offset,
		aligned_tag_name_read_size,
		g_cache_file_debug_globals.debug_tag_name_buffer //cache_file_memory_globals->debug_tag_name_buffer
	);

	if (!is_compatible)
	{
		goto cache_file_header_invalid;
	}

	// Read tag name offsets

	uint32 aligned_tag_name_offset_read_size = cache_file_align_read_size_to_cache_page(sizeof(int32) * cache_file_memory_globals->header.debug_tag_name_count);
	ASSERT(aligned_tag_name_offset_read_size <= sizeof(g_cache_file_debug_globals.debug_tag_name_offsets));
	is_compatible = cache_file_blocking_read(
		NONE,
		cache_file_memory_globals->header.tag_name_offset,
		aligned_tag_name_offset_read_size,
		g_cache_file_debug_globals.debug_tag_name_offsets //cache_file_memory_globals->tag_name_offsets
	);

	if (!is_compatible)
	{
		goto cache_file_header_invalid;
	}

	// Change the pointers in the header to point to the new locations in memory
	tag_header->tag_group_link_set = (s_tag_group_link*)&tag_header[1];
	tag_header->tag_instances = (cache_file_tag_instance*)(((char*)&tag_header[1]) + sizeof(s_tag_group_link) * tag_header->tag_group_link_set_count);

	// These are used for the inlined functions of tag data operations i.e tag_block_get
	*Memory::GetAddress<cache_file_tag_instance**>(0x47cd50, 0x4A29B8) = tag_header->tag_instances;
	*Memory::GetAddress<uint32*>(0x47cd54, 0x4A29BC) = cache_file_memory_globals->tag_cache_base_address;

	if(tag_header->tag_count >= FIRST_SHARED_TAG_INSTANCE_INDEX && !scenario_tags_load_process_shared_tags())
	{
		error(_error_immediate, "failed to load shared tag instances");
		is_compatible = false;
	}

	cache_file_memory_globals->tags_loaded = true;
	tag_index = tag_header->scenario_index;
scenario_tags_load_internal_end:
	if (!is_compatible)
	{
		error(_error_immediate, "failed to load tags for cache file");

		if (cache_file_memory_globals->tag_cache_base_address)
		{
			CSERIES_FREE((int32*)(cache_file_memory_globals->tag_cache_base_address - *(int32*)(cache_file_memory_globals->tag_cache_base_address - 4)));
			cache_file_memory_globals = NULL;
		}
		cache_file_close();

		ASSERT(tag_index == NONE);
	}

	if (tag_index != NONE)
	{
		global_scenario_index_set(tag_index);
		set_global_scenario((scenario*)tag_get_fast(/*'scnr',*/ tag_index));
		scenario_set_game_globals((s_game_globals*)tag_get_fast(/*'matg',*/ cache_file_memory_globals->tags_header->globals_index));

		initialize_runtime_sound_gestalt_definition();

		tag_injection_scenario_load_setup(cache_header->tag_size + cache_header->tag_offset_mask);
		is_compatible = true;
	}

	return is_compatible;
}

void* tag_get(
	uint32 expected_group_tag,
	int32 tag_index)
{
	cache_file_tag_instance const* tag_instance = cache_file_tag_instance_try_and_get_unsafe(tag_index);
	s_tag_group_link* group = tag_group_get_link_set(tag_instance->group_tag);

	ASSERT(group);

	{
		char group_name[16];
		char expected_group_name[16];

		SUPRESS_UNUSED(group_name);
		SUPRESS_UNUSED(expected_group_name);

		vassert(
			group->child == expected_group_tag || group->parent_2 == expected_group_tag || group->parent == expected_group_tag,
			"expected tag group '%s' but got '%s' for %08x",
			tag_to_string(tag_instance->group_tag, group_name),
			tag_to_string(expected_group_tag, expected_group_name),
			tag_index);
	}

	vassert(tag_instance->data_offset!=NONE && *tag_data_get() + tag_instance->data_offset!=NULL, "can't call tag_get() on a cache file tag with a NULL base address", NULL);

	return tag_data_get_from_instance(tag_instance);
}

datum tag_loaded(
	uint32 group_tag,
	char const* name)
{
	const s_cache_file_memory_globals* g_cache_file_memory_globals = cache_file_memory_globals_get();
	datum result = NONE;

	if (g_cache_file_memory_globals->tags_loaded)
	{
		const cache_file_tag_instance* global_tag_instances = global_tag_instances_get();

		ASSERT(global_tag_instances);

		for (int32 i = 0; i < g_cache_file_memory_globals->tags_header->tag_count; ++i)
		{
			const cache_file_tag_instance* tag_instance = &global_tag_instances[i];

			if (group_tag == tag_instance->group_tag)
			{
				const char* tag_name = tag_get_name(tag_instance->tag_index);
				if (!csstricmp(name, tag_name))
				{
					result = tag_instance->tag_index;
					break;
				}
			}
		}
	}
	
	// CARTO ADDITION:
	// Parse the injected tag instances when checking if a tag is loaded
	if (result == NONE)
	{
		const uint16 last_injected_index = k_first_injected_datum + g_tag_injection_manager->get_table()->get_entry_count();
		const cache_file_tag_instance* global_tag_instances = global_tag_instances_get();

		for (uint16 i = k_first_injected_datum; i < last_injected_index; ++i)
		{
			const cache_file_tag_instance* tag_instance = &global_tag_instances[i];
			if (group_tag == tag_instance->group_tag)
			{
				const char* tag_name = tag_get_name(tag_instance->tag_index);
				if (!csstricmp(name, tag_name))
				{
					result = tag_instance->tag_index;
					break;
				}
			}
		}
	}

	return result;
}

const char* tag_get_name(datum tag_index)
{
	uint16 tag_name_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_index);

	s_cache_file_memory_globals* g_cache_file_memory_globals = cache_file_memory_globals_get();
	ASSERT(g_cache_file_memory_globals->tags_loaded);
	
	// We added a second check if the first one fails, since we're going to be passing it tag indexes in the injected tag area
	ASSERT(IN_RANGE(tag_name_index, 0, g_cache_file_memory_globals->header.debug_tag_name_count - 1) || IN_RANGE(tag_name_index - k_first_injected_datum, 0, g_cache_file_memory_globals->header.debug_tag_name_count - 1));

	int32 tag_name_offset = g_cache_file_debug_globals.debug_tag_name_offsets[tag_name_index];

	ASSERT(IN_RANGE(tag_name_offset, 0, NUMBEROF(g_cache_file_debug_globals.debug_tag_name_buffer) - 1));

	return &g_cache_file_debug_globals.debug_tag_name_buffer[tag_name_offset];
}

void tag_add_name(datum tag_index, const char* name)
{
	uint16 tag_name_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_index);

	s_cache_file_memory_globals* cache_file_memory_globals = cache_file_memory_globals_get();
	ASSERT(cache_file_memory_globals->tags_loaded);

	int32 new_name_offset = cache_file_memory_globals->header.tag_name_buffer_size;

	int32 string_length = csstrnlen(name, MAX_PATH) + 1;

	csstrncpy(&g_cache_file_debug_globals.debug_tag_name_buffer[new_name_offset], name, string_length);
	g_cache_file_debug_globals.debug_tag_name_offsets[tag_name_index] = new_name_offset;

	++cache_file_memory_globals->header.debug_tag_name_count;
	cache_file_memory_globals->header.tag_name_buffer_size += string_length;
}

cache_file_tag_instance* tag_instance_get(datum tag_index)
{
	cache_file_tag_instance* global_tag_instances = global_tag_instances_get();
	return &global_tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_index)];
}

/* private code */

bool __cdecl read_shared_resource_database(e_shared_resource_database_type database_type, int32 unused_flags, uint32 offset, uint32 size, void* out_buffer, bool async)
{
	return INVOKE(0x64CC7, 0x4CD1E, read_shared_resource_database, database_type, unused_flags, offset, size, out_buffer, async);
}

void* tag_get_safe(tag_group group, datum tag_index)
{
	if (DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_index) > 0u)
	{
		s_cache_file_memory_globals* cache_file_memory = cache_file_memory_globals_get();
		cache_file_tag_instance* tag_instance = &cache_file_memory->tags_header->tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_index)];

		if (tag_index == tag_instance->tag_index)
		{
			s_tag_group_link* group_link = tag_group_get_link_set(tag_instance->group_tag);

			if (group_link)
			{
				if (group_link->child == group|| group_link->parent_2 == group || group_link->parent == group)
				{
					return (void*)((char*)cache_file_memory->tag_cache_base_address + tag_instance->data_offset);
				}
			}
		}
	}

	return nullptr;
}
